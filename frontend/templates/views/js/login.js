// login.js
console.log("Login");

function initLoginForm() {
  const form = document.getElementById("form-login");

  if (!form) {
    console.error("⚠️ No se encontró el formulario #form-login");
    return;
  }

  console.log("✅ Formulario LOGIN encontrado, configurando submit...");

  form.addEventListener("submit", async (e) => {
    e.preventDefault(); // 🔒 Evita que recargue el index.html
    console.log("🚀 Enviando formulario login...");

    const email = document.getElementById("email").value;
    const password = document.getElementById("password").value;

    $('#email').val('');
    $('#password').val('');
    //console.log(email);
    //console.log(password);

    try {
      const response = await fetch(window.API_BASE_URL +"/login", {
        method: "POST",
        headers: { "Content-Type": "application/x-www-form-urlencoded" },
        body: `email=${encodeURIComponent(email)}&password=${encodeURIComponent(password)}`
      });

      const data = await response.json();
      console.log("📦 Respuesta del backend:", data);
      console.log("HTTP Status:", response.status);
      console.log("response.ok:", response.ok);
      console.log("data.status:", data.status);

      if (response.ok && data.status === "success") {
        console.log("Ingresando...");
        var mensaje = `<br><div class="alert alert-success alert-dismissible">
                                    <a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
                                        <p align=center><span class="glyphicon glyphicon-ok" aria-hidden="true"></span> Bienvenido/a, aguarde un instante...</p></div>`;
        document.getElementById('message_login').innerHTML = mensaje;
        //alert("Bienvenido. Aguarde un instante...");
        localStorage.setItem("user", JSON.stringify(data.user));
        setTimeout(function() { $(".close").click(); }, 3000);
        setTimeout(function() { window.location.href = "/dashboard.html"; }, 4000);


      } else {
          var mensaje = `<br><div class="alert alert-danger alert-dismissible">
                                    <a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
                                        <p align=center><span class="glyphicon glyphicon-warning-sign" aria-hidden="true"></span> ${data.message || "❌  Error Desconocido"}</p></div>`;
          document.getElementById('message_login').innerHTML = mensaje;
          setTimeout(function() { $(".close").click(); }, 4000);
      }
    } catch (err) {
      console.error("💥 Error en fetch:", err);
      alert("No se pudo conectar al servidor backend");
    }
  });
}

// Esperar a que el DOM del fragmento esté cargado
document.addEventListener("DOMContentLoaded", initLoginForm);

// Fallback por si el fragmento fue insertado dinámicamente (sin nuevo DOMContentLoaded)
setTimeout(initLoginForm, 500);
