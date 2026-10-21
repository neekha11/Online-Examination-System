const form = document.getElementById("login-form");
      const error = document.getElementById("form-error");
      const pw = document.getElementById("password");
      const toggle = document.querySelector(".toggle-pw");

      toggle.addEventListener("click", () => {
        const show = pw.type === "password";
        pw.type = show ? "text" : "password";
        toggle.setAttribute("aria-pressed", show);
        toggle.setAttribute(
          "aria-label",
          show ? "Hide password" : "Show password",
        );
      });

      form.addEventListener("submit", (e) => {
        e.preventDefault();
        const ok = form.user.value.trim() && pw.value;
        error.classList.toggle("show", !ok);
        if (ok) {
          // TODO: send credentials to your backend, then redirect
          // window.location.href = 'admin-dashboard.html';
        }
      });