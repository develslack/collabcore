console.log("Password");

function initPasswordForm() {
  const form = document.getElementById("form-password");

  if (!form) {
    console.error("⚠️ No se encontró el formulario #form-password");
    return;
  }

  console.log("✅ Formulario CAMBIO DE PASSWORD encontrado, configurando submit...");

  form.addEventListener("submit", async (e) => {
    e.preventDefault(); // 🔒 Evita que recargue el index.html
    console.log("🚀 Enviando formulario login...");

    const email = document.getElementById("email").value;
    const pwd_1 = document.getElementById("password_1").value;
    const pwd_2 = document.getElementById("password_2").value;

    $('#email').val('');
    $('#password_1').val('');
    $('#password_2').val('');
    //console.log(email);
    //console.log(password);

    try {
      const response = await fetch(window.API_BASE_URL +"/password", {
        method: "POST",
        headers: { "Content-Type": "application/x-www-form-urlencoded" },
        body: `email=${encodeURIComponent(email)}&password_1=${encodeURIComponent(pwd_1)}&password_2=${encodeURIComponent(pwd_2)}`
      });

      const data = await response.json();
      console.log("📦 Respuesta del backend:", data);
      console.log("HTTP Status:", response.status);
      console.log("response.ok:", response.ok);
      console.log("data.status:", data.status);

      if (response.ok && data.status === "ok") {
        console.log("Actualización de password exitosa...");
        alert("Actualización de password exitosa...");
        setTimeout(function() { loadView("/views/login.html"); }, 3000);

      } else {
        alert("❌ " + (data.message || "Error desconocido"));
      }
    } catch (err) {
      console.error("💥 Error en fetch:", err);
      alert("No se pudo conectar al servidor backend");
    }
  });
}

// Esperar a que el DOM del fragmento esté cargado
document.addEventListener("DOMContentLoaded", initPasswordForm);

// Fallback por si el fragmento fue insertado dinámicamente (sin nuevo DOMContentLoaded)
setTimeout(initPasswordForm, 500);
