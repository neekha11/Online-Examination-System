// Student self-registration -> POST /api/student/register
(function () {
  const form = document.getElementById("reg-form");
  const err = document.getElementById("form-error");
  const ok = document.getElementById("form-ok");
  const btn = form.querySelector("button");
  const $ = (id) => document.getElementById(id);

  function show(el, text) {
    err.classList.remove("show"); ok.classList.remove("show");
    el.textContent = text;
    el.classList.add("show");
  }

  function check(v) {
    if (!v.name || !v.department) return "Please fill in every field.";
    if (/,/.test(v.name + v.department)) return "Name and department cannot contain commas.";
    if (!/^[A-Za-z0-9_]{3,30}$/.test(v.username)) return "Username must be 3-30 letters, digits or underscores.";
    if (v.password.length < 8 || !/[A-Za-z]/.test(v.password) || !/\d/.test(v.password))
      return "Password needs at least 8 characters, with a letter and a digit.";
    if (v.password !== v.confirm) return "The two passwords do not match.";
    return "";
  }

  form.addEventListener("submit", async (e) => {
    e.preventDefault();
    const v = {
      name: $("name").value.trim(), username: $("username").value.trim(), password: $("password").value,
      confirm: $("confirm").value, department: $("dept").value.trim(), year: $("year").value,
    };
    const problem = check(v);
    if (problem) return show(err, problem);

    btn.disabled = true;
    try {
      const res = await fetch("/api/student/register", {
        method: "POST",
        body: new URLSearchParams({ name: v.name, username: v.username, password: v.password, department: v.department, year: v.year }),
      });
      const data = await res.json().catch(() => ({}));
      if (!res.ok) throw new Error(data.error || "Registration failed.");
      show(ok, "Account created. Redirecting to sign in…");
      setTimeout(() => (window.location.href = "student-login.html"), 1500);
    } catch (e2) {
      show(err, e2.message);
      btn.disabled = false;
    }
  });
})();
