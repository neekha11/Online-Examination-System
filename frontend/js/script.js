// Shared logic for student-login.html and admin-login.html
(function () {
  "use strict";

  const form = document.getElementById("login-form");
  if (!form) return;

  const role = form.dataset.role; // "student" or "admin"
  const error = document.getElementById("form-error");
  const userInput = document.getElementById("user");
  const pw = document.getElementById("password");
  const toggle = document.querySelector(".toggle-pw");
  const defaultMessage = error.textContent.trim();

  function showError(message) {
    error.textContent = message;
    error.classList.add("show");
  }

  function hideError() {
    error.classList.remove("show");
  }

  // Show / hide password
  toggle.addEventListener("click", () => {
    const show = pw.type === "password";
    pw.type = show ? "text" : "password";
    toggle.setAttribute("aria-pressed", String(show));
    toggle.setAttribute("aria-label", show ? "Hide password" : "Show password");
  });

  // Sends the credentials to the C++ server and stores the session token.
  async function submitLogin(role, username, password) {
    const res = await fetch(`/api/${role}/login`, {
      method: "POST",
      body: new URLSearchParams({ username, password }),
    });
    const data = await res.json().catch(() => ({}));
    if (!res.ok) return { ok: false, message: data.error || "Login failed." };

    sessionStorage.setItem(
      "examera_session",
      JSON.stringify({ token: data.token, role, user: data.student || data.admin }),
    );
    return { ok: true, redirect: `${role}-dashboard.html` };
  }

  form.addEventListener("submit", async (e) => {
    e.preventDefault();

    const username = userInput.value.trim();
    const password = pw.value;

    if (!username || !password) {
      showError(defaultMessage);
      return;
    }

    hideError();

    try {
      const result = await submitLogin(role, username, password);
      if (result.ok) {
        window.location.href = result.redirect;
      } else {
        showError(result.message);
      }
    } catch (err) {
      showError("Unable to reach the server. Please try again.");
    }
  });
})();