// Admin dashboard: create exams, manage questions, view results.
(async function () {
  const session = Api.requireRole("admin");
  if (!session) return;
  const { esc, call } = Api;
  const $ = (sel, root = document) => root.querySelector(sel);

  $(".avatar").textContent = "AD";
  $(".name-text").textContent = session.user.name;
  $(".signout").addEventListener("click", async (e) => {
    e.preventDefault();
    await Api.logout();
    window.location.href = "admin-login.html";
  });

  const main = $(".dash-main");
  main.innerHTML = `
    <div class="dash-title"><div><h1>Examination overview</h1>
      <p>Create exams, add questions and review how students performed.</p></div></div>
    <section class="stats" id="stats"></section>
    <div class="grid">
      <section class="panel"><div class="panel-head"><h2>Examinations</h2></div><div id="exam-table"></div></section>
      <section class="panel"><div class="panel-head"><h2>Create examination</h2></div>
        <form class="form-grid" id="exam-form">
          <div class="f"><label for="t">Title</label><input id="t" required maxlength="100" placeholder="e.g. Data Structures Basics" /></div>
          <div class="f-row">
            <div class="f"><label for="d">Duration (minutes)</label><input id="d" type="number" min="1" max="600" value="30" required /></div>
            <div class="f"><label for="p">Planned questions</label><input id="p" type="number" min="1" max="200" value="10" required /></div>
          </div>
          <button class="btn-primary" type="submit">Create exam</button><div class="msg" id="exam-msg"></div>
        </form></section>
    </div>
    <section class="panel hidden" id="work" style="margin-top:18px">
      <div class="panel-head"><h2 id="work-title"></h2><a href="#" id="work-close">Close</a></div>
      <div class="tabs"><button class="tab on" data-tab="q">Questions</button><button class="tab" data-tab="r">Results</button></div>
      <div id="tab-q"><div id="q-list"></div>
        <h3 style="font-size:12px;font-weight:500;margin:16px 0 10px">Add a question</h3>
        <form class="form-grid" id="q-form">
          <div class="f-row">
            <div class="f"><label for="qt">Type</label><select id="qt"><option value="MCQ">Multiple choice</option><option value="TF">True / False</option></select></div>
            <div class="f"><label for="qm">Marks</label><input id="qm" type="number" min="1" max="100" value="1" /></div>
          </div>
          <div class="f"><label for="qx">Question text (no commas)</label><textarea id="qx" required maxlength="300"></textarea></div>
          <div id="mcq-opts" class="form-grid">
            <div class="f-row"><div class="f"><label>Option A</label><input id="oa" /></div><div class="f"><label>Option B</label><input id="ob" /></div></div>
            <div class="f-row"><div class="f"><label>Option C</label><input id="oc" /></div><div class="f"><label>Option D</label><input id="od" /></div></div>
          </div>
          <div class="f"><label for="qc">Correct answer</label><select id="qc"></select></div>
          <button class="btn-primary" type="submit">Add question</button><div class="msg" id="q-msg"></div>
        </form></div>
      <div id="tab-r" class="hidden"></div>
    </section>`;

  let current = null;   // exam being managed

  function say(id, text, ok) {
    const el = document.getElementById(id);
    el.textContent = text;
    el.className = "msg " + (ok ? "ok" : "bad");
  }

  async function loadExams() {
    const data = await call("GET", "/api/admin/exams");
    const stat = (label, value, note) => `<div class="stat"><span>${label}</span><strong>${value}</strong><small>${note}</small></div>`;
    const totalQ = data.exams.reduce((n, e) => n + e.questions, 0);
    $("#stats").innerHTML = stat("Examinations", data.exams.length, "Created so far") + stat("Questions", totalQ, "Across all exams") +
      stat("Registered students", data.students, "Can sign in") + stat("Attempts", data.attempts, "Submitted exams");

    $("#exam-table").innerHTML = data.exams.length
      ? `<div class="table-wrap"><table><thead><tr><th>Exam</th><th>Time</th><th>Questions</th><th>Taken</th><th></th></tr></thead><tbody>` +
        data.exams.map((e) => `<tr><td class="name">${esc(e.title)}<br><span class="muted">ID ${e.id}</span></td>
          <td class="muted">${e.duration} min</td>
          <td>${e.questions}/${e.planned} <span class="muted">· ${e.totalMarks} marks</span></td><td>${e.attempts}</td>
          <td><button class="btn-ghost" data-open="${e.id}" data-title="${esc(e.title)}">Manage</button></td></tr>`).join("") +
        `</tbody></table></div>`
      : `<div class="empty"><b>No examinations yet</b>Create your first one using the form.</div>`;
  }

  async function loadQuestions() {
    const list = await call("GET", `/api/admin/exams/${current.id}/questions`);
    $("#q-list").innerHTML = list.length
      ? list.map(({ question: q, correctAnswer }, i) => `<div class="q-item"><div>
          <h4>${i + 1}. ${esc(q.text)}</h4>
          <p>${q.type === "MCQ" ? q.options.map((o, n) => `${String.fromCharCode(65 + n)}) ${esc(o)}`).join("  ·  ") + " — " : ""}Correct: <b>${correctAnswer}</b> · ${q.marks} mark${q.marks === 1 ? "" : "s"}</p></div>
          <button class="btn-ghost danger" data-del="${q.id}">Delete</button></div>`).join("")
      : `<div class="empty"><b>No questions yet</b>Add the first question below.</div>`;
  }

  async function loadResults() {
    const rows = await call("GET", `/api/admin/exams/${current.id}/results`);
    $("#tab-r").innerHTML = rows.length
      ? `<div class="table-wrap"><table><thead><tr><th>Student</th><th>Score</th><th>Grade</th><th>Status</th><th>Submitted</th></tr></thead><tbody>` +
        rows.map(({ studentName, result: r }) => `<tr><td class="name">${esc(studentName)}</td><td>${r.score}/${r.totalMarks} (${r.percentage}%)</td>
          <td>${esc(r.grade)}</td><td><span class="chip ${r.passed ? "pass" : "draft"}">${r.passed ? "Passed" : "Failed"}</span></td>
          <td class="muted">${esc(r.submittedAt)}</td></tr>`).join("") + `</tbody></table></div>`
      : `<div class="empty"><b>No attempts yet</b>Results appear here once students submit.</div>`;
  }

  function setType() {
    const mcq = $("#qt").value === "MCQ";
    $("#mcq-opts").classList.toggle("hidden", !mcq);
    $("#qc").innerHTML = mcq ? "<option>A</option><option>B</option><option>C</option><option>D</option>"
      : `<option value="TRUE">True</option><option value="FALSE">False</option>`;
  }
  $("#qt").addEventListener("change", setType);
  setType();

  $("#exam-table").addEventListener("click", async (e) => {
    const b = e.target.closest("[data-open]");
    if (!b) return;
    current = { id: b.dataset.open, title: b.dataset.title };
    $("#work-title").textContent = `${current.title} (ID ${current.id})`;
    $("#work").classList.remove("hidden");
    $("#work").scrollIntoView({ behavior: "smooth", block: "start" });
    await Promise.all([loadQuestions(), loadResults()]);
  });
  $("#work-close").addEventListener("click", (e) => { e.preventDefault(); $("#work").classList.add("hidden"); });
  document.querySelectorAll(".tab").forEach((t) => t.addEventListener("click", () => {
    document.querySelectorAll(".tab").forEach((x) => x.classList.toggle("on", x === t));
    $("#tab-q").classList.toggle("hidden", t.dataset.tab !== "q");
    $("#tab-r").classList.toggle("hidden", t.dataset.tab !== "r");
  }));

  $("#exam-form").addEventListener("submit", async (e) => {
    e.preventDefault();
    try {
      await call("POST", "/api/admin/exams", { title: $("#t").value.trim(), duration: $("#d").value, planned: $("#p").value });
      say("exam-msg", "Exam created. Open it with Manage to add questions.", true);
      e.target.reset(); $("#d").value = 30; $("#p").value = 10;
      await loadExams();
    } catch (err) { say("exam-msg", err.message, false); }
  });

  $("#q-form").addEventListener("submit", async (e) => {
    e.preventDefault();
    try {
      await call("POST", "/api/admin/questions", {
        examId: current.id, type: $("#qt").value, text: $("#qx").value.trim(), marks: $("#qm").value,
        a: $("#oa").value.trim(), b: $("#ob").value.trim(), c: $("#oc").value.trim(), d: $("#od").value.trim(), correct: $("#qc").value,
      });
      say("q-msg", "Question added.", true);
      ["#qx", "#oa", "#ob", "#oc", "#od"].forEach((s) => ($(s).value = ""));
      await Promise.all([loadQuestions(), loadExams()]);
    } catch (err) { say("q-msg", err.message, false); }
  });

  $("#q-list").addEventListener("click", async (e) => {
    const b = e.target.closest("[data-del]");
    if (!b || !confirm("Delete this question?")) return;
    try { await call("DELETE", `/api/admin/questions/${b.dataset.del}`); await Promise.all([loadQuestions(), loadExams()]); }
    catch (err) { say("q-msg", err.message, false); }
  });

  try { await loadExams(); } catch (err) { $("#stats").innerHTML = `<div class="empty">${esc(err.message)}</div>`; }
})();
