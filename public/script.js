document.addEventListener("DOMContentLoaded", () => {
  const btn = document.getElementById("actionBtn");

  if (btn) {
    btn.addEventListener("click", () => {
      alert("Hello! The JavaScript is working!");
      document.body.style.backgroundColor = "#e0ffe0";
    });
  }
});
