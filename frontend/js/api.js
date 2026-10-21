// Shared helpers: session storage + calls to the C++ server.
const Api = (() => {
  const KEY = "examera_session";

  const get = () => {
    try { return JSON.parse(sessionStorage.getItem(KEY)); } catch (e) { return null; }
  };

  // Redirects to the login page unless a session with this role exists.
  function requireRole(role) {
    const s = get();
    if (!s || s.role !== role) {
      window.location.href = `${role}-login.html`;
      return null;
    }
    return s;
  }

  async function call(method, path, params) {
    const s = get();
    const options = { method, headers: {} };
    if (s) options.headers.Authorization = "Bearer " + s.token;
    if (params) options.body = new URLSearchParams(params);

    const res = await fetch(path, options);
    const data = await res.json().catch(() => ({}));
    if (res.status === 401 && s) {            // session expired or server restarted
      sessionStorage.removeItem(KEY);
      window.location.href = `${s.role}-login.html`;
    }
    if (!res.ok) throw new Error(data.error || "Request failed.");
    return data;
  }

  async function logout() {
    try { await call("POST", "/api/logout"); } catch (e) {}
    sessionStorage.removeItem(KEY);
  }

  const esc = (t) =>
    String(t).replace(/[&<>"']/g, (c) => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;" }[c]));

  return { get, requireRole, call, logout, esc };
})();
