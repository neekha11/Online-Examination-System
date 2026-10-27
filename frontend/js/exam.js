// Exam page: intro -> one question at a time with a question palette -> result and review.
(async function () {
  const session = Api.requireRole("student");
  if (!session) return;

  const examId = new URLSearchParams(location.search).get("id");
  const view = document.getElementById("view");
  const timerEl = document.getElementById("timer");
  const { esc } = Api;
  const notice = (msg, cls = "") => `<div class="notice ${cls}">${esc(msg)}</div>`;

  let deadline = 0, ticker = null, submitting = false, cur = 0;
  let questions = [];
  const answers = {};            // questionId -> saved answer
  const visited = new Set();     // questionIds opened at least once
  const marked = new Set();      // questionIds marked for review

  // ---------- 1. intro ----------
  let exam;
  try {
    const exams = await Api.call("GET", "/api/exams");
    exam = exams.find((e) => String(e.id) === examId);
  } catch (err) {
    view.innerHTML = notice(err.message, "error");
    return;
  }
  if (!exam) { view.innerHTML = notice("Exam not found.", "error"); return; }
  if (exam.attempted) { view.innerHTML = notice("You have already attempted this exam.") + backLink(); return; }

  view.innerHTML = `
    <h1>${esc(exam.title)}</h1>
    <p class="exam-meta">${exam.duration} minutes · ${exam.questions} questions</p>
    <section class="panel q-card">
      <div class="panel-head"><h2>Instructions</h2></div>
      <ul class="instructions">
        <li><b>One attempt only.</b> Your result is saved when you submit.</li>
        <li><b>Move freely.</b> Use the question palette on the right to jump to any question.</li>
        <li><b>Save &amp; Next</b> keeps your answer and opens the next question. <b>Clear Response</b> removes it.</li>
        <li><b>Mark for Review</b> flags a question to revisit. Marked questions that have an answer are still evaluated.</li>
        <li><b>The timer runs on the server.</b> Closing the page does not pause it, and your answers are submitted automatically at zero.</li>
      </ul>
      <div class="actions"><button class="primary-btn" id="start">${exam.inProgress ? "Resume exam" : "Start exam"}</button>
        <a class="hint" href="student-dashboard.html">Not now</a></div>
      <div id="msg"></div>
    </section>`;
  document.getElementById("start").addEventListener("click", start);

  function backLink() {
    return `<div class="actions"><a class="primary-btn" href="student-dashboard.html">Back to dashboard</a></div>`;
  }

  // ---------- 2. exam ----------
  async function start() {
    try {
      const data = await Api.call("POST", `/api/exams/${examId}/start`);
      questions = data.questions;
      deadline = Date.now() + data.secondsLeft * 1000;
      document.body.classList.add("exam-mode");
      renderShell(data.exam);
      show(0);
      timerEl.hidden = false;
      tick();
      ticker = setInterval(tick, 500);
      window.addEventListener("beforeunload", warnLeave);
    } catch (err) {
      document.getElementById("msg").innerHTML = notice(err.message, "error");
    }
  }

  const warnLeave = (e) => { if (!submitting) { e.preventDefault(); e.returnValue = ""; } };
  const $ = (sel) => view.querySelector(sel);

  function renderShell(info) {
    const name = session.user.name;
    view.innerHTML = `
      <div class="cbt-title">${esc(info.title)}</div>
      <div class="cbt">
        <section class="cbt-main">
          <div class="cbt-head"><span class="qt" id="qtype"></span><span id="qmarks"></span></div>
          <h2 class="cbt-qno" id="qno"></h2>
          <div class="cbt-body" id="qbody"></div>
          <div class="cbt-actions">
            <button class="btn-light" id="mark"></button>
            <button class="btn-light" id="clear">Clear Response</button>
            <span class="spacer"></span>
            <button class="btn-light" id="prev">Previous</button>
            <button class="btn-blue" id="next"></button>
          </div>
        </section>
        <aside class="cbt-side">
          <div class="who"><span class="who-av">${esc(name[0].toUpperCase())}</span><b>${esc(name)}</b></div>
          <div class="legend" id="legend"></div>
          <h3 class="pal-title">Choose a question</h3>
          <div class="pal" id="pal"></div>
          <button class="btn-blue wide" id="submit">Submit exam</button>
          <div id="msg"></div>
        </aside>
      </div>`;

    $("#qbody").addEventListener("input", onAnswer);
    $("#pal").addEventListener("click", (e) => {
      const b = e.target.closest("[data-i]");
      if (b) show(Number(b.dataset.i));
    });
    $("#next").addEventListener("click", () => show(Math.min(cur + 1, questions.length - 1)));
    $("#prev").addEventListener("click", () => show(Math.max(cur - 1, 0)));
    $("#clear").addEventListener("click", () => { delete answers[questions[cur].id]; show(cur); });
    $("#mark").addEventListener("click", () => {
      const id = questions[cur].id;
      if (marked.has(id)) marked.delete(id); else marked.add(id);
      show(marked.has(id) ? Math.min(cur + 1, questions.length - 1) : cur);
    });
    $("#submit").addEventListener("click", confirmSubmit);
  }

  function stateOf(i) {
    const id = questions[i].id;
    if (!visited.has(id)) return "nv";
    const has = answers[id] !== undefined;
    if (has && marked.has(id)) return "am";
    if (marked.has(id)) return "mk";
    return has ? "ans" : "na";
  }

  function counts() {
    const c = { ans: 0, na: 0, nv: 0, mk: 0, am: 0 };
    questions.forEach((q, i) => c[stateOf(i)]++);
    return c;
  }

  function renderPalette() {
    $("#pal").innerHTML = questions.map((q, i) =>
      `<button class="pq ${stateOf(i)}${i === cur ? " cur" : ""}" data-i="${i}" aria-label="Question ${i + 1}">${i + 1}</button>`).join("");
    const c = counts();
    $("#legend").innerHTML = [["ans", "Answered"], ["na", "Not answered"], ["nv", "Not visited"], ["mk", "Marked for review"], ["am", "Answered &amp; marked"]]
      .map(([k, label]) => `<span class="item"><span class="pq sm ${k}">${c[k]}</span>${label}</span>`).join("");
  }

  function show(i) {
    cur = i;
    const q = questions[i];
    visited.add(q.id);

    const kind = { MCQ: "Multiple Choice Question", TF: "True / False Question", DESC: "Descriptive Question" }[q.type] || "Question";
    $("#qtype").textContent = "Question type: " + kind;
    $("#qmarks").innerHTML = `Marks: <b>${q.marks}</b>`;
    $("#qno").textContent = `Question No. ${i + 1} of ${questions.length}`;
    $("#qbody").innerHTML = `<p class="cbt-text">${esc(q.text)}</p><div class="options">${optionsFor(q)}</div>`;
    $("#mark").textContent = marked.has(q.id) ? "Unmark Review" : "Mark for Review & Next";
    $("#prev").disabled = i === 0;
    $("#next").textContent = i === questions.length - 1 ? "Save" : "Save & Next";
    renderPalette();
  }

  function optionsFor(q) {
    const saved = answers[q.id];
    if (q.type === "DESC")
      return `<textarea class="essay" name="q${q.id}" rows="8" maxlength="2000" placeholder="Type your answer here">${esc(saved || "")}</textarea>`;
    const choices = q.type === "TF"
      ? [["TRUE", "True"], ["FALSE", "False"]]
      : q.options.map((o, i) => [String.fromCharCode(65 + i), o]);
    return choices.map(([value, label]) =>
      `<label class="option"><input type="radio" name="q${q.id}" value="${value}"${saved === value ? " checked" : ""} /><span>${q.type === "TF" ? "" : value + ") "}${esc(label)}</span></label>`).join("");
  }

  function onAnswer(e) {
    const q = questions[cur];
    const value = e.target.value.trim();
    if (value) answers[q.id] = value; else delete answers[q.id];
    renderPalette();
  }

  function tick() {
    const left = Math.max(0, Math.ceil((deadline - Date.now()) / 1000));
    timerEl.textContent = `${String(Math.floor(left / 60)).padStart(2, "0")}:${String(left % 60).padStart(2, "0")}`;
    timerEl.classList.toggle("warn", left <= 60);
    if (left === 0 && !submitting) submit(true);
  }

  // ---------- 3. submit + result ----------
  function confirmSubmit() {
    const c = counts();
    const text = `Answered: ${c.ans + c.am}\nNot answered: ${c.na}\nNot visited: ${c.nv}\nMarked for review: ${c.mk + c.am}\n\nSubmit your exam now?`;
    if (confirm(text)) submit();
  }

  async function submit(auto) {
    submitting = true;
    clearInterval(ticker);
    const params = {};
    Object.entries(answers).forEach(([id, a]) => (params["a_" + id] = a));
    view.querySelectorAll("button").forEach((b) => (b.disabled = true));
    try {
      const data = await Api.call("POST", `/api/exams/${examId}/submit`, params);
      showResult(data, auto);
    } catch (err) {
      submitting = false;
      view.querySelectorAll("button").forEach((b) => (b.disabled = false));
      $("#msg").innerHTML = notice(err.message, "error");
    }
  }

  function showResult({ result: r, review, late }, auto) {
    timerEl.hidden = true;
    document.body.classList.remove("exam-mode");
    window.removeEventListener("beforeunload", warnLeave);
    const msg = late ? notice("Your submission arrived after the time limit, so no answers were counted.", "error")
      : auto ? notice("Time ran out, so your answers were submitted automatically.") : "";

    view.innerHTML = `<h1>${esc(r.examTitle)}</h1>${msg}${r.pending > 0 ? notice(`${r.pending} written answer${r.pending === 1 ? " is" : "s are"} waiting to be marked, so your score may increase.`) : ""}
      <section class="panel q-card score-card">
        <div class="big">${r.score} / ${r.totalMarks}</div>
        <div class="grade"><span class="chip ${r.pending > 0 ? "scheduled" : r.passed ? "pass" : "draft"}">${r.pending > 0 ? "Pending review" : r.passed ? "Passed" : "Failed"} · Grade ${esc(r.grade)}</span></div>
        <div class="exam-meta">${r.percentage}% · submitted ${esc(r.submittedAt)}</div>
        <div class="result-grid">
          <div><b>${r.correct}</b>Correct</div><div><b>${r.attempted}</b>Attempted</div><div><b>${r.totalQuestions}</b>Questions</div>
        </div></section>
      <div class="actions"><a class="primary-btn" href="student-dashboard.html">Back to dashboard</a></div>
      <h2 style="margin-top:18px;font-size:14px;font-weight:500">Answer review</h2>
      ${review.map((item, i) => reviewCard(item, i)).join("")}`;
    window.scrollTo(0, 0);
  }

  function reviewCard({ question: q, yourAnswer, correctAnswer, isCorrect }, i) {
    if (q.type === "DESC")
      return `<section class="panel q-card"><h3><small>QUESTION ${i + 1} · ${yourAnswer ? "AWAITING REVIEW" : "NOT ANSWERED"}</small>${esc(q.text)}</h3>
        <div class="essay-answer">${yourAnswer ? esc(yourAnswer) : "No answer given."}</div></section>`;
    const choices = q.type === "TF"
      ? [["TRUE", "True"], ["FALSE", "False"]]
      : q.options.map((o, n) => [String.fromCharCode(65 + n), o]);
    const rows = choices.map(([value, label]) => {
      const cls = value === correctAnswer ? "right" : value === yourAnswer ? "wrong" : "";
      return `<div class="option ${cls}"><span>${q.type === "TF" ? "" : value + ") "}${esc(label)}</span></div>`;
    }).join("");
    const verdict = !yourAnswer ? "Not answered" : isCorrect ? "Correct" : "Wrong";
    return `<section class="panel q-card"><h3><small>QUESTION ${i + 1} · ${verdict.toUpperCase()}</small>${esc(q.text)}</h3>
      <div class="options">${rows}</div></section>`;
  }
})();
