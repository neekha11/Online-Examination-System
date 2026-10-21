// Fills the student dashboard with real data from the server.
(async function () {
  const session = Api.requireRole("student");
  if (!session) return;

  const $ = (sel) => document.querySelector(sel);
  const name = session.user.name;
  const initials = name.split(" ").map((w) => w[0]).slice(0, 2).join("").toUpperCase();

  $(".avatar").textContent = initials;
  $(".name-text").textContent = name;
  $(".dash-title h1").textContent = `Welcome back, ${name.split(" ")[0]}.`;
  $(".signout").addEventListener("click", async (e) => {
    e.preventDefault();
    await Api.logout();
    window.location.href = "student-login.html";
  });

  let exams, results;
  try {
    [exams, results] = await Promise.all([
      Api.call("GET", "/api/exams"),
      Api.call("GET", "/api/student/results"),
    ]);
  } catch (err) {
    $(".dash-title p").textContent = "Could not load your data: " + err.message;
    return;
  }

  const available = exams.filter((e) => !e.attempted && e.questions > 0);
  const average = results.length
    ? Math.round(results.reduce((sum, r) => sum + parseFloat(r.percentage), 0) / results.length)
    : 0;

  $(".dash-title p").textContent = available.length
    ? `You have ${available.length} examination${available.length === 1 ? "" : "s"} ready to take.`
    : results.length ? "You're all caught up. New exams will appear here." : "No examinations are available yet.";
  const heads = document.querySelectorAll(".panel-head h2");
  heads[0].textContent = "Your examinations";
  const viewAll = document.querySelectorAll(".panel-head a");
  viewAll.forEach((a) => a.remove());

  const stats = document.querySelectorAll(".stat");
  const setStat = (i, label, value, note) => {
    stats[i].querySelector("span").textContent = label;
    stats[i].querySelector("strong").textContent = value;
    stats[i].querySelector("small").textContent = note;
  };
  setStat(0, "Available exams", available.length, "Ready to start");
  setStat(1, "Completed", results.length, "Attempts so far");
  setStat(2, "Average score", results.length ? average + "%" : "–", "Across all results");

  // Exam list
  $(".exam-list").innerHTML = exams.length
    ? exams.map((e) => {
        let action = `<a class="row-btn solid" href="exam.html?id=${e.id}">${e.inProgress ? "Resume" : "Start exam"}</a>`;
        if (e.attempted) action = `<span class="chip done">Completed</span>`;
        else if (e.questions === 0) action = `<span class="chip draft">No questions yet</span>`;
        return `<div class="exam-row">
          <div class="date-box">EXAM<b>${e.id}</b></div>
          <div class="exam-info"><h3>${Api.esc(e.title)}</h3>
            <p>${e.duration} minutes · ${e.questions} question${e.questions === 1 ? "" : "s"}</p></div>
          ${action}</div>`;
      }).join("")
    : `<div class="empty"><b>No examinations yet</b>Check back soon.</div>`;

  // Results table
  $("tbody").innerHTML = results.length
    ? results.slice().reverse().map((r) => `<tr>
        <td class="name">${Api.esc(r.examTitle)}</td>
        <td class="muted">${Api.esc(r.submittedAt)}</td>
        <td>${r.score}/${r.totalMarks} (${r.percentage}%)</td>
        <td><span class="chip ${r.passed ? "pass" : "draft"}">${r.passed ? "Passed" : "Failed"}</span></td></tr>`).join("")
    : `<tr><td colspan="4"><div class="empty"><b>No results yet</b>Your scores will appear here after your first exam.</div></td></tr>`;

  // Progress bars (replaces the sample subjects)
  const bars = $(".bars");
  if (bars) {
    bars.parentElement.querySelector("h2").textContent = "Your scores";
    bars.innerHTML = results.length
      ? results.map((r) => `<div><div class="bar-label"><span>${Api.esc(r.examTitle)}</span><span>${Math.round(r.percentage)}%</span></div>
          <div class="bar"><i style="width:${r.percentage}%"></i></div></div>`).join("")
      : `<div class="empty">Scores appear here after your first exam.</div>`;
  }
  const tips = document.querySelector(".instructions");
  if (tips) tips.innerHTML = `<li><b>One attempt per exam.</b> Your result is saved when you submit.</li>
    <li><b>The timer runs on the server.</b> Closing the page will not pause it.</li>
    <li><b>Pass mark is 40%.</b> Unanswered questions score zero.</li>`;
})();
