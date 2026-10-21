// Exam page: intro -> timed questions -> result and answer review.
(async function () {
  const session = Api.requireRole("student");
  if (!session) return;

  const examId = new URLSearchParams(location.search).get("id");
  const view = document.getElementById("view");
  const timerEl = document.getElementById("timer");
  const { esc } = Api;
  let deadline = 0, ticker = null, submitting = false, questions = [];

  const notice = (msg, cls = "") => `<div class="notice ${cls}">${esc(msg)}</div>`;

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
      <div class="panel-head"><h2>Before you begin</h2></div>
      <ul class="instructions">
        <li><b>One attempt only.</b> Your result is saved when you submit.</li>
        <li><b>The timer runs on the server.</b> Closing the page does not pause it.</li>
        <li><b>Auto-submit.</b> When time reaches zero your answers are submitted for you.</li>
        <li><b>Skipping is allowed.</b> Unanswered questions score zero.</li>
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
      renderExam(data.exam);
      timerEl.hidden = false;
      tick();
      ticker = setInterval(tick, 500);
      window.addEventListener("beforeunload", warnLeave);
    } catch (err) {
      document.getElementById("msg").innerHTML = notice(err.message, "error");
    }
  }

  const warnLeave = (e) => { if (!submitting) { e.preventDefault(); e.returnValue = ""; } };

  function renderExam(info) {
    view.innerHTML = `<h1>${esc(info.title)}</h1>
      <p class="exam-meta">Answer as many questions as you can. Choose one option per question.</p>
      ${questions.map((q, i) => `
        <section class="panel q-card"><h3><small>QUESTION ${i + 1} · ${q.marks} MARK${q.marks === 1 ? "" : "S"}</small>${esc(q.text)}</h3>
          <div class="options">${optionsFor(q)}</div></section>`).join("")}
      <div class="actions"><button class="primary-btn" id="submit">Submit exam</button>
        <button class="primary-btn ghost" id="clear" type="button">Clear all answers</button>
        <span class="hint" id="count"></span></div><div id="msg"></div>`;

    view.addEventListener("change", updateCount);
    document.getElementById("submit").addEventListener("click", () => {
      const left = questions.length - Object.keys(collect()).length;
      const text = left ? `${left} question${left === 1 ? " is" : "s are"} unanswered. Submit anyway?` : "Submit your exam now?";
      if (confirm(text)) submit();
    });
    document.getElementById("clear").addEventListener("click", () => {
      view.querySelectorAll("input:checked").forEach((i) => (i.checked = false));
      updateCount();
    });
    updateCount();
  }

  function optionsFor(q) {
    const choices = q.type === "TF"
      ? [["TRUE", "True"], ["FALSE", "False"]]
      : q.options.map((o, i) => [String.fromCharCode(65 + i), o]);
    return choices.map(([value, label]) =>
      `<label class="option"><input type="radio" name="q${q.id}" value="${value}" /><span>${q.type === "TF" ? "" : value + ") "}${esc(label)}</span></label>`).join("");
  }

  function collect() {
    const answers = {};
    questions.forEach((q) => {
      const picked = view.querySelector(`input[name="q${q.id}"]:checked`);
      if (picked) answers[q.id] = picked.value;
    });
    return answers;
  }

  function updateCount() {
    const el = document.getElementById("count");
    if (el) el.textContent = `${Object.keys(collect()).length} of ${questions.length} answered`;
  }

  function tick() {
    const left = Math.max(0, Math.ceil((deadline - Date.now()) / 1000));
    timerEl.textContent = `${String(Math.floor(left / 60)).padStart(2, "0")}:${String(left % 60).padStart(2, "0")}`;
    timerEl.classList.toggle("warn", left <= 60);
    if (left === 0 && !submitting) submit(true);
  }

  // ---------- 3. submit + result ----------
  async function submit(auto) {
    submitting = true;
    clearInterval(ticker);
    const answers = Object.entries(collect()).map(([id, a]) => `${id}:${a}`).join(",");
    view.querySelectorAll("button").forEach((b) => (b.disabled = true));
    try {
      const data = await Api.call("POST", `/api/exams/${examId}/submit`, { answers });
      showResult(data, auto);
    } catch (err) {
      submitting = false;
      view.insertAdjacentHTML("beforeend", notice(err.message, "error"));
      view.querySelectorAll("button").forEach((b) => (b.disabled = false));
    }
  }

  function showResult({ result: r, review, late }, auto) {
    timerEl.hidden = true;
    window.removeEventListener("beforeunload", warnLeave);
    const msg = late ? notice("Your submission arrived after the time limit, so no answers were counted.", "error")
      : auto ? notice("Time ran out, so your answers were submitted automatically.") : "";

    view.innerHTML = `<h1>${esc(r.examTitle)}</h1>${msg}
      <section class="panel q-card score-card">
        <div class="big">${r.score} / ${r.totalMarks}</div>
        <div class="grade"><span class="chip ${r.passed ? "pass" : "draft"}">${r.passed ? "Passed" : "Failed"} · Grade ${esc(r.grade)}</span></div>
        <div class="exam-meta">${r.percentage}% · submitted ${esc(r.submittedAt)}</div>
        <div class="result-grid">
          <div><b>${r.correct}</b>Correct</div><div><b>${r.attempted}</b>Attempted</div><div><b>${r.totalQuestions}</b>Questions</div>
        </div></section>
      <div class="actions"><a class="primary-btn" href="student-dashboard.html">Back to dashboard</a></div>
      <h2 style="margin-top:18px;font-size:14px;font-weight:500">Answer review</h2>
      ${review.map((item, i) => reviewCard(item, i)).join("")}`;
  }

  function reviewCard({ question: q, yourAnswer, correctAnswer, isCorrect }, i) {
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
