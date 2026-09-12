// app.js
// =========================================================================
// CONFIGURACIÓN DE ENTORNO (DEV vs PROD)
// =========================================================================
const isDev = (window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1');
window.API_BASE_URL = isDev ? 'http://localhost:4000' : '/api';

// Añadir esta línea
window.VIEWS_PATH = isDev ? '/views' : '';

console.log("🌐 Entorno detectado. URL Base de la API configurada como:", window.API_BASE_URL);

// =========================================================================
// INDICADOR VISUAL DE ENTORNO (FLOTANTE)
// =========================================================================

const envBadge = document.createElement("div");

// Configurar el texto y el ícono según el entorno
envBadge.innerHTML = isDev
    ? '<span class="glyphicon glyphicon-wrench"></span> ENTORNO DESARROLLO'
    : '<span class="glyphicon glyphicon-globe"></span> ENTORNO PRODUCCIÓN';

// Aplicar estilos flotantes (Abajo a la derecha)
envBadge.style.position = 'fixed';
envBadge.style.bottom = '15px';
envBadge.style.right = '15px';
envBadge.style.padding = '8px 15px';
envBadge.style.borderRadius = '20px';
envBadge.style.color = '#fff';
envBadge.style.backgroundColor = isDev ? '#d35400' : '#27ae60'; // Naranja para Dev, Verde para Prod
envBadge.style.zIndex = '9999';
envBadge.style.fontSize = '12px';
envBadge.style.fontWeight = 'bold';
envBadge.style.boxShadow = '0 4px 6px rgba(0,0,0,0.3)';
envBadge.style.pointerEvents = 'none'; // Evita que bloquee clics en elementos debajo

// Inyectar en el documento
document.body.appendChild(envBadge);

// =========================================================================

document.addEventListener("DOMContentLoaded", () => {
  console.log("Panel de Control...");

  // Verificar si existe un usuario en sesión
  const user = JSON.parse(localStorage.getItem("user"));

  if (!user) {
    console.warn("⚠️ No hay sesión activa, redirigiendo al inicio de sesión...");
    alert("⚠️ No hay sesión activa, redirigiendo al inicio de sesión...");
    setTimeout(function() { window.location.href = "/"; }, 4000);
    return;
  } else if (user.token.length == 128) {
    let navBar = `<nav class="navbar navbar-inverse">
                        <div class="container-fluid">
                            <div class="navbar-header">
                                <button type="button" class="navbar-toggle" data-toggle="collapse" data-target="#myNavbar">
                                    <span class="icon-bar"></span>
                                    <span class="icon-bar"></span>
                                    <span class="icon-bar"></span>
                                </button>
                                <a class="navbar-brand" href="#">
                                    <button type="button" class="btn btn-default btn-sm"><span class="glyphicon glyphicon-dashboard"></span> Dashboard </button></a>
                            </div>
                            <div class="collapse navbar-collapse" id="myNavbar">
                                <ul class="nav navbar-nav">
                                    <li id="link-home"><a href="#" data-toggle="tooltip" title="Limpar pantalla">
                                        <button type="button" class="btn btn-warning btn-sm"><span class="glyphicon glyphicon-home"></span> Home </button></a></li>
                                </ul>
                                <ul class="nav navbar-nav navbar-right">
                                    <li><a href="#" id="dashboard-logout" data-toggle="tooltip" title="Salir de la aplicación">
                                        <button type="button" class="btn btn-danger btn-sm"><span class="glyphicon glyphicon-log-out"></span> Salir </button></a></li>
                                    <li><a href="#" id="user-my-data" data-toggle="tooltip" title="Datos personales" data-id="${user.nombre}">
                                        <button type="button" class="btn btn-info btn-sm"><span class="glyphicon glyphicon-user"></span> ${user.nombre} </button></a></li>
                                </ul>
                            </div>
                        </div>
                    </nav><br>

                    <div class="container-fluid">
                        <div id="dashboard_messages"></div>
                        <div class="row content">
                            <div class="col-sm-2 sidenav">
                               <div class="panel panel-primary">
                                        <div class="panel-heading"><span class="glyphicon glyphicon-list-alt" aria-hidden="true"></span> Menú</div>
                                            <div class="panel-body">
                                                <div class="panel-group" id="accordion">
                                                    <!-- Menú 1: Sistema (Solo Admin) -->
                                                    <div class="panel panel-default" id="menu-sistema-container">
                                                        <div class="panel-heading">
                                                            <h4 class="panel-title">
                                                            <a data-toggle="collapse" data-parent="#accordion" href="#collapse1"><span class="glyphicon glyphicon-cog" aria-hidden="true"></span> Sistema</a>
                                                            </h4>
                                                        </div>
                                                        <div id="collapse1" class="panel-collapse collapse">
                                                            <div class="panel-body">
                                                                <div class="list-group">
                                                                <a href="#" class="list-group-item" id="link-usuarios" data-toggle="tooltip" title="Usuarios"><span class="glyphicon glyphicon-user" aria-hidden="true"></span> Usuarios</a>
                                                                <a href="#" class="list-group-item" id="link-roles" data-toggle="tooltip" title="Listar Roles"><span class="glyphicon glyphicon-asterisk" aria-hidden="true"></span> Roles</a>
                                                                <a href="#" class="list-group-item" id="link-modulos" data-toggle="tooltip" title="Listar Módulos"><span class="glyphicon glyphicon-th-list" aria-hidden="true"></span> Módulos</a>
                                                                <a href="#" class="list-group-item" id="link-documentacion_tecnica" data-toggle="tooltip" title="Listar Documentación Técnica"><span class="glyphicon glyphicon-book" aria-hidden="true"></span> Documentación Técnica</a>
                                                                </div>
                                                            </div>
                                                        </div>
                                                    </div>

                                                    <!-- Menú 2: Carga de Lotes (Público) -->
                                                    <div class="panel panel-default">
                                                        <div class="panel-heading">
                                                            <h4 class="panel-title">
                                                            <a data-toggle="collapse" data-parent="#accordion" href="#collapse2">
                                                                <span class="glyphicon glyphicon-circle-arrow-down" aria-hidden="true"></span> Carga de Lotes</a></h4>
                                                        </div>
                                                        <div id="collapse2" class="panel-collapse collapse">
                                                            <div class="panel-body">
                                                                <div class="list-group">
                                                                <a href="#" class="list-group-item" id="link-parametros_basicos" data-toggle="tooltip" title="Cargar Parámetros Básicos">
                                                                    <span class="glyphicon glyphicon-th-large" aria-hidden="true"></span> Parámetros Básicos</a>
                                                                <a href="#" class="list-group-item" id="link-ch" data-toggle="tooltip" title="Listar CH">
                                                                    <span class="glyphicon glyphicon-th-large" aria-hidden="true"></span> CH (Cabezal de Haberes)</a>
                                                                <a href="#" class="list-group-item" id="link-dp" data-toggle="tooltip" title="Listar DP">
                                                                    <span class="glyphicon glyphicon-th-large" aria-hidden="true"></span> DP (Datos de Personal)</a>
                                                                <a href="#" class="list-group-item" id="link-lh1" data-toggle="tooltip" title="Listar LH1">
                                                                    <span class="glyphicon glyphicon-th-large" aria-hidden="true"></span> LH1 (Liquidación de Haberes 1)</a>
                                                                <a href="#" class="list-group-item" id="link-lh2" data-toggle="tooltip" title="Listar LH2">
                                                                    <span class="glyphicon glyphicon-th-large" aria-hidden="true"></span> LH2 (Liquidación de Haberes 2)</a>
                                                                <a href="#" class="list-group-item" id="link-administracion_lotes" data-toggle="tooltip" title="Administración de Lotes">
                                                                    <span class="glyphicon glyphicon-cog" aria-hidden="true"></span> Administración de Lotes</a>
                                                                </div>
                                                            </div>
                                                        </div>
                                                    </div>

                                                    <!-- Menú 3: Tablas Maestro (Solo Admin) -->
                                                    <div class="panel panel-default" id="menu-maestros-container">
                                                        <div class="panel-heading">
                                                            <h4 class="panel-title">
                                                            <a data-toggle="collapse" data-parent="#accordion" href="#collapse3"><span class="glyphicon glyphicon-tree-conifer" aria-hidden="true"></span> Tablas Maestro</a></h4>
                                                        </div>
                                                        <div id="collapse3" class="panel-collapse collapse">
                                                            <div class="panel-body">
                                                                <div class="list-group">
                                                                <a href="#" class="list-group-item" id="link-tablas_maestro" data-toggle="tooltip" title="Listar Tablas Maestro">
                                                                    <span class="glyphicon glyphicon-star" aria-hidden="true"></span> Tablas Maestro</a>
                                                                </div>
                                                            </div>
                                                        </div>
                                                    </div>

                                                </div>
                                            </div>
                                        </div>
                                    </div>
                            <div class="col-sm-10 text-left"><br>
                                <div id="dashboard_views"></div>
                            </div>
                        </div>
                    </div>`;

    // Inyectar contenido en el contenedor
    const dashNav = document.getElementById("navBar");
    dashNav.innerHTML = navBar;

    // 🛡️ ADUANA VISUAL: Ocultamos estéticamente las opciones administrativas del menú a los no-admins
    if (parseInt(user.rol_id) !== 1) {
        const menuSistema = document.getElementById("menu-sistema-container");
        const menuMaestros = document.getElementById("menu-maestros-container");
        if (menuSistema) menuSistema.style.display = "none";
        if (menuMaestros) menuMaestros.style.display = "none";
    }

    document.addEventListener("click", (e) => {
      const btnEdit = e.target.closest("#user-my-data");
      if (btnEdit) {
        e.preventDefault();
        // Leemos el nombre directamente del objeto user en localStorage
        const user = JSON.parse(localStorage.getItem("user"));
        const user_name = user ? user.nombre : null;

        console.log("👤 Iniciando carga de mis datos para:", user_name);

        if (user_name) {
          loadDataUserForm(user_name);
        }
      }
    });
  }

  console.log("👤 Usuario autenticado: ", user.nombre);
  console.log("👤 Usuario token: ", user.token);

  // Acción de logout
  const logoutBtn = document.getElementById("dashboard-logout");
  if (logoutBtn) {
    logoutBtn.addEventListener("click", () => {
      // Eliminar datos de sesión
      localStorage.removeItem("user");

      // Redirigir al login (index.html)
      console.log("🚪 Cerrando sesión...");
      var message = `<div class="container-fluid">
                        <div class="alert alert-info">
                            <p align="center"><span class="glyphicon glyphicon-exclamation-sign" aria-hidden="true"></span> <strong>Aguarde un instante!</strong> Estamos cerrando la sesión.</p>
                        </div>
                    </div>`;

      document.getElementById('dashboard_messages').innerHTML = message;
      setTimeout(function() { window.location.href = "/"; }, 3000);
    });
  }

  const viewsContainer = document.getElementById("dashboard_views");

  // Carga inicial
  loadDashboardView(window.VIEWS_PATH + "/home.html");

  // Atribuir eventos de navegación
  document.getElementById("dashboard-logout")?.addEventListener("click", (e) => {
    e.preventDefault();
    setTimeout(function() { window.location.href = "../index.html"; }, 3000);
  });

  // ======================================================================================================================== //
  // ESPACIO DE USUARIOS //
  // ======================================================================================================================== //
  document.getElementById("link-usuarios")?.addEventListener("click", (e) => {
    e.preventDefault();
    loadDashboardView(window.VIEWS_PATH + "/usuarios.html");
  });

  // ======================================================================================================================== //
  // ESPACIO DE PARAMETROS BASICOS Y TABLAS DE ARCHIVOS DE LOTES//
  // ======================================================================================================================== //
  document.getElementById("link-parametros_basicos")?.addEventListener("click", (e) => {
    e.preventDefault();
    loadDashboardView(window.VIEWS_PATH + "/parametros_basicos/parametros_basicos.html");
  });

  document.getElementById("link-ch")?.addEventListener("click", (e) => {
    e.preventDefault();
    loadDashboardView(window.VIEWS_PATH + "/ch/listado_ch.html");
  });

  document.getElementById("link-dp")?.addEventListener("click", (e) => {
    e.preventDefault();
    loadDashboardView(window.VIEWS_PATH + "/dp/listado_dp.html");
  });

  document.getElementById("link-lh1")?.addEventListener("click", (e) => {
    e.preventDefault();
    loadDashboardView(window.VIEWS_PATH + "/lh1/listado_lh1.html");
  });

  document.getElementById("link-lh2")?.addEventListener("click", (e) => {
    e.preventDefault();
    loadDashboardView(window.VIEWS_PATH + "/lh2/listado_lh2.html");
  });

  document.getElementById("link-administracion_lotes")?.addEventListener("click", (e) => {
    e.preventDefault();
    loadDashboardView(window.VIEWS_PATH + "/administracion_lotes/administracion_lotes.html");
  });

  document.getElementById("link-documentacion_tecnica")?.addEventListener("click", (e) => {
    e.preventDefault();
    loadDashboardView(window.VIEWS_PATH + "/documentacion_tecnica.html");
  });



  // ======================================================================================================================== //

  // Click en el botón para añadir usuario
  document.addEventListener("click", (e) => {
    const target = e.target.closest("#add-user-form");
    if (target) {
      e.preventDefault();
      console.log("🟢 Botón 'Añadir Usuario' clickeado");
      loadDashboardView(window.VIEWS_PATH + "/register.html");
    }
  });

  // Click para el formulario de cambio de rol del usuario
  document.addEventListener("click", (e) => {
    const target = e.target.closest('#editar_rol_usuario');
    if (target) {
      e.preventDefault();
      console.log("🟢 Botón 'Editar Rol' clickeado");
      loadDashboardView(window.VIEWS_PATH + "/roles/rol_usuario.html");
    }
  });

  // ======================================================================================================================== //
  // ESPACIO DE MODULOS //
  // ======================================================================================================================== //
  document.getElementById("link-modulos")?.addEventListener("click", (e) => {
    e.preventDefault();
    loadDashboardView(window.VIEWS_PATH + "/modulos/modulos.html");
  });

  // Click en el botón para añadir módulo
  document.addEventListener("click", (e) => {
    const target = e.target.closest("#add-modulo-form");
    if (target) {
      e.preventDefault();
      console.log("🟢 Botón 'Añadir Módulo' clickeado");
      loadDashboardView(window.VIEWS_PATH + "/modulos/nuevo_modulo.html");
    }
  });

  // ======================================================================================================================== //
  // ESPACIO DE ROLES //
  // ======================================================================================================================== //
  document.getElementById("link-roles")?.addEventListener("click", (e) => {
    e.preventDefault();
    loadDashboardView(window.VIEWS_PATH + "/roles/roles.html");
  });

  // Click en el botón para añadir rol/función
  document.addEventListener("click", (e) => {
    const target = e.target.closest("#add-rol-form");
    if (target) {
      e.preventDefault();
      console.log("🟢 Botón 'Añadir Roles' clickeado");
      loadDashboardView(window.VIEWS_PATH + "/roles/nuevo_rol.html");
    }
  });

  document.getElementById("link-home")?.addEventListener("click", (e) => {
    e.preventDefault();
    loadDashboardView(window.VIEWS_PATH + "/home.html");
  });

  // ======================================================================================================================== //
  // FUNCIÓN CENTRAL DESPACHADORA CON ADUANA PERIMETRAL (KERNEL-ROUTE-FIREWALL)
  // ======================================================================================================================== //
  async function loadDashboardView(viewPath) {
    const container = document.getElementById("dashboard_views");
    if (!container) return;

    // 1. EXTRAER SESIÓN DEL LOCALSTORAGE
    const currentSession = JSON.parse(localStorage.getItem("user"));
    const rolUsuario = currentSession ? parseInt(currentSession.rol_id) : 0;

    // 2. DEFINIR PALABRAS CLAVE ASOCIADAS A MÓDULOS DE ADMINISTRACIÓN (SISTEMA Y TABLAS MAESTRO)
    const modulosAdministrativos = [
      "usuarios", "roles", "modulos", "register", "rol_usuario", "nuevo_rol", "nuevo_modulo",
      "tablas_maestro", "instituciones", "actividades", "agrupamientos", "cargos_directivos",
      "discapacidades", "disciplinas", "escalafones", "estado_civil", "fuente_financiamiento",
      "identidad_genero", "jurisdicciones", "marca_estado", "nacionalidades", "nivel_educativo",
      "niveles", "programas", "proyectos", "remunerativo_bonificable", "sanciones_disciplinarias",
      "sexos", "subjurisdicciones", "nueva_subjurisdiccion", "tipo_accesos", "tipo_conceptos",
      "tipo_documentos", "tipo_horarios", "tipo_licencias", "tipo_plantas", "tipo_unidades_fisicas",
      "ubicaciones_geograficas", "unidades_organizativas", "subprogramas", "entidades"
    ];

    // Comprobar si el viewPath actual tiene alguna de las palabras restringidas
    const esRutaRestringida = modulosAdministrativos.some(keyword => viewPath.includes(keyword));

    // 🛡️ CORTAFUEGOS NATIVO FRONTEND: Si el usuario NO es rol_id = 1, bloqueamos la inyección
    if (esRutaRestringida && rolUsuario !== 1) {
      console.warn(`🛑 [CORTAFUEGOS]: Intento de acceso denegado a la ruta: ${viewPath}`);
      container.innerHTML = `
        <br>
        <div class="container-fluid">
          <div class="jumbotron" style="background-color: #fcf8e3; border: 1px solid #fbeed5; color: #c09853; border-radius: 6px;">
            <h2 class="text-danger">
              <span class="glyphicon glyphicon-lock" aria-hidden="true"></span> <strong>Acceso Restringido</strong>
            </h2>
            <hr style="border-top-color: #f7ecb5;">
            <p style="font-size: 16px;"><strong>Lo sentimos:</strong> Su usuario no tiene permisos de acceso a este módulo.</p>
            <p style="font-size: 14px;">Si cree que se trata de un error, por favor contacte al Administrador Supremo del Sistema.</p>
            <br>
            <button class="btn btn-warning" onclick="window.loadDashboardView('/views/home.html')">
              <span class="glyphicon glyphicon-home"></span> Volver al Inicio
            </button>
          </div>
        </div>
      `;
      return; // Freno de mano: Evitamos continuar cargando el HTML y los scripts JS
    }

    try {
      const response = await fetch(viewPath);
      if (!response.ok) throw new Error("Vista no encontrada");

      const html = await response.text();
      container.innerHTML = html;

      // 🔹 Cargar JS asociado dinámicamente de forma limpia
      if (viewPath.includes("usuarios")) {
        loadDashboardScript("/js/usuarios.js");
      }
      else if (viewPath.includes("register")) {
        loadDashboardScript("/js/register.js");
      }
      else if (viewPath.includes("password")) {
        loadDashboardScript("/js/password.js");
      }
      else if (viewPath.includes("modulos") || viewPath.includes("editar_modulo")) {
        loadDashboardScript("/js/modulos.js");
      }
      else if (viewPath.includes("instituciones") || viewPath.includes("editar_institucion")) {
        loadDashboardScript("/js/instituciones.js");
      }
      else if (viewPath.includes("actividades") || viewPath.includes("editar_actividad")) {
        loadDashboardScript("/js/actividades.js");
      }
      else if (viewPath.includes("agrupamientos") || viewPath.includes("editar_agrupamiento")) {
        loadDashboardScript("/js/agrupamientos.js");
      }
      else if (viewPath.includes("cargos_directivos") || viewPath.includes("editar_cargo_directivo")) {
        loadDashboardScript("/js/cargos_directivos.js");
      }
      else if (viewPath.includes("discapacidades") || viewPath.includes("editar_discapacidad")) {
        loadDashboardScript("/js/discapacidades.js");
      }
      else if (viewPath.includes("disciplinas") || viewPath.includes("editar_disciplina")) {
        loadDashboardScript("/js/disciplinas.js");
      }
      else if (viewPath.includes("escalafones") || viewPath.includes("editar_escalafon")) {
        loadDashboardScript("/js/escalafones.js");
      }
      else if (viewPath.includes("estado_civil") || viewPath.includes("editar_estado_civil")) {
        loadDashboardScript("/js/estado_civil.js");
      }
      else if (viewPath.includes("fuente_financiamiento") || viewPath.includes("editar_fuente_financiamiento")) {
        loadDashboardScript("/js/fuente_financiamiento.js");
      }
      else if (viewPath.includes("identidad_genero") || viewPath.includes("editar_identidad_genero")) {
        loadDashboardScript("/js/identidad_genero.js");
      }
      else if (viewPath.includes("subjurisdicciones")) {
        loadDashboardScript("/js/subjurisdicciones.js");
      }
      else if (viewPath.includes("jurisdicciones") || viewPath.includes("editar_jurisdicciones")) {
        loadDashboardScript("/js/jurisdicciones.js");
      }
      else if (viewPath.includes("marca_estado") || viewPath.includes("editar_marca_estado")) {
        loadDashboardScript("/js/marca_estado.js");
      }
      else if (viewPath.includes("nacionalidades") || viewPath.includes("editar_nacionalidad")) {
        loadDashboardScript("/js/nacionalidades.js");
      }
      else if (viewPath.includes("nivel_educativo") || viewPath.includes("editar_nivel_educativo")) {
        loadDashboardScript("/js/nivel_educativo.js");
      }
      else if (viewPath.includes("niveles") || viewPath.includes("editar_niveles")) {
        loadDashboardScript("/js/niveles.js");
      }
      else if (viewPath.includes("subprogramas")) {
        loadDashboardScript("/js/subprogramas.js");
      }
      else if (viewPath.includes("programas") || viewPath.includes("editar_programas")) {
        loadDashboardScript("/js/programas.js");
      }
      else if (viewPath.includes("proyectos") || viewPath.includes("editar_proyectos")) {
        loadDashboardScript("/js/proyectos.js");
      }
      else if (viewPath.includes("remunerativo_bonificable") || viewPath.includes("editar_remunerativos_bonificables")) {
        loadDashboardScript("/js/remunerativo_bonificable.js");
      }
      else if (viewPath.includes("sanciones_disciplinarias") || viewPath.includes("editar_sanciones_disciplinarias")) {
        loadDashboardScript("/js/sanciones_disciplinarias.js");
      }
      else if (viewPath.includes("tipo_accesos")) {
        loadDashboardScript("/js/tipo_accesos.js");
      }
      else if (viewPath.includes("tipo_conceptos")) {
        loadDashboardScript("/js/tipo_conceptos.js");
      }
      else if (viewPath.includes("tipo_documentos")) {
        loadDashboardScript("/js/tipo_documentos.js");
      }
      else if (viewPath.includes("tipo_horarios")) {
        loadDashboardScript("/js/tipo_horarios.js");
      }
      else if (viewPath.includes("tipo_licencias")) {
        loadDashboardScript("/js/tipo_licencias.js");
      }
      else if (viewPath.includes("tipo_plantas")) {
        loadDashboardScript("/js/tipo_plantas.js");
      }
      else if (viewPath.includes("tipo_unidades_fisicas")) {
        loadDashboardScript("/js/tipo_unidades_fisicas.js");
      }
      else if (viewPath.includes("sexos") || viewPath.includes("editar_sexo")) {
        loadDashboardScript("/js/sexos.js");
      }
      else if (viewPath.includes("ubicaciones_geograficas")) {
        loadDashboardScript("/js/ubicaciones_geograficas.js");
      }
      else if (viewPath.includes("unidades_organizativas")) {
        loadDashboardScript("/js/unidades_organizativas.js");
      }
      else if (viewPath.includes("entidades")) {
        loadDashboardScript("/js/entidades.js");
      }
      else if (viewPath.includes("parametros_basicos")) {
        loadDashboardScript("/js/parametros_basicos.js");
      }
      else if (viewPath.includes("ch")) {
        loadDashboardScript("/js/ch.js");
      }
      else if (viewPath.includes("dp")) {
        loadDashboardScript("/js/dp.js");
      }
      else if (viewPath.includes("lh1")) {
        loadDashboardScript("/js/lh1.js");
      }
      else if (viewPath.includes("lh2")) {
        loadDashboardScript("/js/lh2.js");
      }
      else if (viewPath.includes("administracion_lotes")) {

          // Carga el script por defecto
            loadDashboardScript("/js/generacion_lote.js");

            // Escucha el cambio de solapas en la vista activa
            $('a[data-toggle="tab"]').off('shown.bs.tab').on('shown.bs.tab', function (e) {
                const targetTab = $(e.target).attr("href");

                if (targetTab === "#home") {
                    loadDashboardScript("/js/generacion_lote.js");
                } else if (targetTab === "#menu1") {
                    loadDashboardScript("/js/copiar_lotes.js");
                }else if (targetTab === "#menu2") {
                    loadDashboardScript("/js/eliminar_lote.js");
                }else if (targetTab === "#menu3") {
                    loadDashboardScript("/js/archivos_externos.js");
                }
            });

      }
      else if (viewPath.includes("roles")) {
        loadDashboardScript("/js/roles.js");
      }

      // 🔥 TRASPASO AUTOMÁTICO DE CONTEXTO: Sincroniza y bloquea los campos de control en la vista inyectada
      setTimeout(() => {
        if (typeof window.sincronizarCamposDeControl === "function") {
          window.sincronizarCamposDeControl();
        }
      }, 50);


    } catch (err) {
      console.error("Error al cargar vista:", err);
    }
  }

  // Función auxiliar: inyecta y recarga scripts en el DOM sin romper el enrutador de estáticos
function loadDashboardScript(src) {
  // 1. Buscamos y removemos cualquier script previo que tenga la misma ruta exacta
  const scriptExistente = document.querySelector(`script[src="${src}"]`);

  if (scriptExistente) {
    scriptExistente.remove();
    console.log("♻️ Reiniciando script:", src);
  }

  // 2. Creamos e inyectamos el script limpio
  const script = document.createElement("script");
  script.type = "text/javascript";
  script.src = src;

  // Manejo de errores de carga
  script.onerror = () => {
    console.error(`💥 Error al cargar el archivo de script: ${src}`);
  };

  document.body.appendChild(script);
}

  // ======================================================================================================================== //
  // 🚀 DELEGACIÓN GLOBAL DE EVENTOS PARA NAVEGACIÓN ASÍNCRONA
  // ======================================================================================================================== //
  document.addEventListener("click", function (e) {
    // Buscamos si el clic se hizo en un ID de los que necesitamos capturar
    const targetLink = e.target.closest(
        "#link-tablas_maestro, \
         #link-instituciones, \
         #link-actividades, \
         #link-agrupamientos, \
         #link-cargos_directivos, \
         #link-discapacidades, \
         #link-disciplinas_estudio, \
         #link-escalafones_completos , \
         #link-estado_civil, \
         #link-fuente_financiamiento, \
         #link-identidad_genero, \
         #link-jurisdicciones, \
         #link-marca_estado, \
         #link-nacionalidades, \
         #link-nivel_educativo, \
         #link-niveles, \
         #link-programas, \
         #link-proyectos, \
         #link-remunerativos_bonificables, \
         #link-sanciones_disciplinarias, \
         #link-sexos, \
         #link-subjurisdicciones, \
         #link-subprogramas, \
         #link-tipo_accesos, \
         #link-tipo_conceptos, \
         #link-tipo_documentos, \
         #link-tipo_horarios, \
         #link-tipo_licencias, \
         #link-tipo_plantas, \
         #link-tipo_unidades_fisicas, \
         #link-ubicaciones_geograficas, \
         #link-unidades_organizativas, \
         #link-entidades, \
         #doc_backend_framework_auth, \
         #doc_backend_framework_backend_server, \
         #doc_backend_framework_commonlib, \
         #doc_backend_framework_db, \
         #doc_backend_framework_handle, \
         #doc_backend_framework_hash, \
         #doc_backend_framework_login, \
         #doc_backend_framework_routes, \
         #doc_backend_framework_session_manager, \
         #doc_backend_framework_system_struct, \
         #doc_backend_framework_users, \
         #doc_backend_framework_arraylist, \
         #doc_backend_framework_frontend_server, \
         #doc_backend_bussiness_logic_actividades_service, \
         #doc_backend_bussiness_logic_agrupamiento_service, \
         #doc_backend_bussiness_logic_cargo_directivo_service, \
         #doc_backend_bussiness_logic_ch_service, \
         #doc_backend_bussiness_logic_copiar_lotes_service, \
         #doc_backend_bussiness_logic_discapacidades_service, \
         #doc_backend_bussiness_logic_disciplina_estudio_service, \
         #doc_backend_bussiness_logic_dp_service, \
         #doc_backend_bussiness_logic_eliminar_lote_service, \
         #doc_backend_bussiness_logic_entidades_service, \
         #doc_backend_bussiness_logic_escalafones_service, \
         #doc_backend_bussiness_logic_estado_civil_service, \
         #doc_backend_bussiness_logic_estado_lote_service, \
         #doc_backend_bussiness_logic_fuente_financiamiento_service, \
         #doc_backend_bussiness_logic_generador_lotes_service, \
         #doc_backend_bussiness_logic_identidad_genero_service, \
         #doc_backend_bussiness_logic_instituciones_service, \
         #doc_backend_bussiness_logic_jurisdicciones_service, \
         #doc_backend_bussiness_logic_lh1_service, \
         #doc_backend_bussiness_logic_lh2_service, \
         #doc_backend_bussiness_logic_marca_estado_service, \
         #doc_backend_bussiness_logic_nacionalidades_service, \
         #doc_backend_bussiness_logic_nivel_educativo_service, \
         #doc_backend_bussiness_logic_niveles_service, \
         #doc_backend_bussiness_logic_parametros_basicos_service, \
         #doc_backend_bussiness_logic_programas_service, \
         #doc_backend_bussiness_logic_proyectos_service, \
         #doc_backend_bussiness_logic_remunerativo_bonificable_service, \
         #doc_backend_bussiness_logic_sancion_disciplinaria_service, \
         #doc_backend_bussiness_logic_sexo_service, \
         #doc_backend_bussiness_logic_subjurisdicciones_service, \
         #doc_backend_bussiness_logic_subprogramas_service, \
         #doc_backend_bussiness_logic_tipo_acceso_service, \
         #doc_backend_bussiness_logic_tipo_concepto_service, \
         #doc_backend_bussiness_logic_tipo_documento_service, \
         #doc_backend_bussiness_logic_tipo_horario_service, \
         #doc_backend_bussiness_logic_tipo_licencia_service, \
         #doc_backend_bussiness_logic_tipo_planta_service, \
         #doc_backend_bussiness_logic_tipo_unidad_fisica_service, \
         #doc_backend_bussiness_logic_ubicacion_geografica_service, \
         #doc_backend_bussiness_logic_unidad_organizativa_service, \
         #doc_backend_bussiness_logic_users_service, \
         #doc_backend_bussiness_logic_validacion_lote_service, \
         #doc_backend_bussiness_logic_verificar_externos_service, \
         #doc_database_schema, \
         #doc_db_gls_usuarios, \
         #doc_db_gls_roles, \
         #doc_db_gls_ch, \
         #doc_db_gls_dp, \
         #doc_db_gls_lh1, \
         #doc_db_gls_lh2, \
         #doc_db_gls_parametros, \
         #doc_db_gls_instituciones, \
         #doc_db_gls_jurisdicciones, \
         #doc_db_gls_sub_jurisdicciones, \
         #doc_db_gls_entidades, \
         #doc_db_gls_programas, \
         #doc_db_gls_subprogramas, \
         #doc_db_gls_proyectos, \
         #doc_db_gls_actividades, \
         #doc_db_gls_unidad_organizativa, \
         #doc_db_gls_ubicacion_geografica, \
         #doc_db_gls_permisos_usuarios, \
         #doc_db_gls_modulos, \
         #doc_db_gls_tipo_documento, \
         #doc_db_gls_identidad_genero, \
         #doc_db_gls_estado_civil, \
         #doc_db_gls_nacionalidad, \
         #doc_db_gls_codigo_discapacidad, \
         #doc_db_gls_codigo_disciplina_estudio, \
         #doc_db_gls_tipo_concepto, \
         #doc_db_gls_remunerativo_bonificable, \
         #doc_db_gls_tipo_unidad_fisica, \
         #doc_db_gls_fuente_financiamiento, \
         #doc_db_gls_tipo_planta, \
         #doc_db_gls_tipo_licencia, \
         #doc_db_gls_sancion_disciplinaria, \
         #doc_db_gls_tipo_horario, \
         #doc_db_gls_tipo_acceso, \
         #doc_db_gls_sexo, \
         #doc_db_gls_nivel_educativo");

    if (targetLink) {
      e.preventDefault(); // Evitamos que la página intente recargarse o saltar

      const viewId = targetLink.id;
      console.log(`🎯 Navegación interceptada dinámicamente desde: ${viewId}`);

      // Despachamos de forma limpia al contenedor principal según el ID del link
      switch (viewId) {
        case "link-tablas_maestro":
          loadDashboardView(window.VIEWS_PATH + "/tablas_maestro.html");
          break;
        case "link-instituciones":
          loadDashboardView(window.VIEWS_PATH + "/instituciones/instituciones.html");
          break;
        case "link-actividades":
          loadDashboardView(window.VIEWS_PATH + "/actividades/actividades.html");
          break;
        case "link-agrupamientos":
          loadDashboardView(window.VIEWS_PATH + "/agrupamientos/agrupamientos.html");
          break;
        case "link-cargos_directivos":
          loadDashboardView(window.VIEWS_PATH + "/cargos_directivos/cargos_directivos.html");
          break;
        case "link-discapacidades":
          loadDashboardView(window.VIEWS_PATH + "/discapacidades/discapacidades.html");
          break;
        case "link-disciplinas_estudio":
          loadDashboardView(window.VIEWS_PATH + "/disciplinas_estudio/disciplinas.html");
          break;
        case "link-escalafones_completos":
          loadDashboardView(window.VIEWS_PATH + "/escalafones/escalafones.html");
          break;
        case "link-estado_civil":
          loadDashboardView(window.VIEWS_PATH + "/estado_civil/estado_civil.html");
          break;
        case "link-fuente_financiamiento":
          loadDashboardView(window.VIEWS_PATH + "/fuente_financiamiento/fuente_financiamiento.html");
          break;
        case "link-identidad_genero":
          loadDashboardView(window.VIEWS_PATH + "/identidad_genero/identidad_genero.html");
          break;
        case "link-jurisdicciones":
          loadDashboardView(window.VIEWS_PATH + "/jurisdicciones/jurisdicciones.html");
          break;
        case "link-marca_estado":
          loadDashboardView(window.VIEWS_PATH + "/marca_estado/marca_estado.html");
          break;
        case "link-nacionalidades":
          loadDashboardView(window.VIEWS_PATH + "/nacionalidades/nacionalidades.html");
          break;
        case "link-nivel_educativo":
          loadDashboardView(window.VIEWS_PATH + "/nivel_educativo/nivel_educativo.html");
          break;
        case "link-niveles":
          loadDashboardView(window.VIEWS_PATH + "/niveles/niveles.html");
          break;
        case "link-programas":
          loadDashboardView(window.VIEWS_PATH + "/programas/programas.html");
          break;
        case "link-proyectos":
          loadDashboardView(window.VIEWS_PATH + "/proyectos/proyectos.html");
          break;
        case "link-remunerativos_bonificables":
          loadDashboardView(window.VIEWS_PATH + "/remunerativos_bonificables/remunerativo_bonificable.html");
          break;
        case "link-sanciones_disciplinarias":
          loadDashboardView(window.VIEWS_PATH + "/sancion_disciplinaria/sanciones_disciplinarias.html");
          break;
        case "link-sexos":
          loadDashboardView(window.VIEWS_PATH + "/sexos/sexos.html");
          break;
        case "link-subjurisdicciones":
          loadDashboardView(window.VIEWS_PATH + "/subjurisdicciones/subjurisdicciones.html");
          break;
        case "link-subprogramas":
          loadDashboardView(window.VIEWS_PATH + "/subprogramas/subprogramas.html");
          break;
        case "link-tipo_accesos":
          loadDashboardView(window.VIEWS_PATH + "/tipo_acceso/tipo_accesos.html");
          break;
        case "link-tipo_conceptos":
          loadDashboardView(window.VIEWS_PATH + "/tipo_concepto/tipo_conceptos.html");
          break;
        case "link-tipo_documentos":
          loadDashboardView(window.VIEWS_PATH + "/tipo_documento/tipo_documentos.html");
          break;
        case "link-tipo_horarios":
          loadDashboardView(window.VIEWS_PATH + "/tipo_horario/tipo_horarios.html");
          break;
        case "link-tipo_licencias":
          loadDashboardView(window.VIEWS_PATH + "/tipo_licencia/tipo_licencias.html");
          break;
        case "link-tipo_plantas":
          loadDashboardView(window.VIEWS_PATH + "/tipo_planta/tipo_plantas.html");
          break;
        case "link-tipo_unidades_fisicas":
          loadDashboardView(window.VIEWS_PATH + "/tipo_unidad_fisica/tipo_unidades_fisicas.html");
          break;
        case "link-ubicaciones_geograficas":
          loadDashboardView(window.VIEWS_PATH + "/ubicacion_geografica/ubicaciones_geograficas.html");
          break;
        case "link-unidades_organizativas":
          loadDashboardView(window.VIEWS_PATH + "/unidad_organizativa/unidades_organizativas.html");
          break;
        case "link-entidades":
          loadDashboardView(window.VIEWS_PATH + "/entidades/entidades.html");
          break;
        case "doc_backend_framework_auth":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_auth.html");
          break;
        case "doc_backend_framework_backend_server":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_backend_server.html");
          break;
        case "doc_backend_framework_commonlib":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_commonlib.html");
          break;
        case "doc_backend_framework_db":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_db.html");
          break;
        case "doc_backend_framework_handle":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_handle.html");
          break;
        case "doc_backend_framework_hash":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_hash.html");
          break;
        case "doc_backend_framework_login":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_login.html");
          break;
        case "doc_backend_framework_routes":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_routes.html");
          break;
        case "doc_backend_framework_session_manager":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_session_manager.html");
          break;
        case "doc_backend_framework_system_struct":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_system_struct.html");
          break;
        case "doc_backend_framework_users":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_users.html");
          break;
        case "doc_backend_framework_arraylist":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_arraylist.html");
          break;
        case "doc_backend_framework_frontend_server":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_frontend_server.html");
          break;
        case "doc_backend_bussiness_logic_actividades_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_actividades_service.html");
          break;
        case "doc_backend_bussiness_logic_agrupamiento_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_agrupamiento_service.html");
          break;
        case "doc_backend_bussiness_logic_cargo_directivo_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_cargo_directivo_service.html");
          break;
        case "doc_backend_bussiness_logic_ch_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_ch_service.html");
          break;
        case "doc_backend_bussiness_logic_dp_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_dp_service.html");
          break;
        case "doc_backend_bussiness_logic_copiar_lotes_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_copiar_lotes_service.html");
          break;
        case "doc_backend_bussiness_logic_discapacidades_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_discapacidades_service.html");
          break;
        case "doc_backend_bussiness_logic_disciplina_estudio_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_disciplina_estudio_service.html");
          break;
        case "doc_backend_bussiness_logic_eliminar_lote_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_eliminar_lote_service.html");
          break;
        case "doc_backend_bussiness_logic_entidades_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_entidades_service.html");
          break;
        case "doc_backend_bussiness_logic_escalafones_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_escalafones_service.html");
          break;
        case "doc_backend_bussiness_logic_estado_civil_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_estado_civil_service.html");
          break;
        case "doc_backend_bussiness_logic_estado_lote_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_estado_lote_service.html");
          break;
        case "doc_backend_bussiness_logic_fuente_financiamiento_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_fuente_financiamiento_service.html");
          break;
        case "doc_backend_bussiness_logic_generador_lotes_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_generador_lotes_service.html");
          break;
        case "doc_backend_bussiness_logic_identidad_genero_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_identidad_genero_service.html");
          break;
        case "doc_backend_bussiness_logic_instituciones_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_instituciones_service.html");
          break;
        case "doc_backend_bussiness_logic_jurisdicciones_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_jurisdicciones_service.html");
          break;
        case "doc_backend_bussiness_logic_lh1_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_lh1_service.html");
          break;
        case "doc_backend_bussiness_logic_lh2_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_lh2_service.html");
          break;
        case "doc_backend_bussiness_logic_marca_estado_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_marca_estado_service.html");
          break;
        case "doc_backend_bussiness_logic_nacionalidades_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_nacionalidades_service.html");
          break;
        case "doc_backend_bussiness_logic_nivel_educativo_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_nivel_educativo_service.html");
          break;
        case "doc_backend_bussiness_logic_niveles_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_niveles_service.html");
          break;
        case "doc_backend_bussiness_logic_parametros_basicos_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_parametros_basicos_service.html");
          break;
        case "doc_backend_bussiness_logic_programas_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_programas_service.html");
          break;
        case "doc_backend_bussiness_logic_proyectos_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_proyectos_service.html");
          break;
        case "doc_backend_bussiness_logic_remunerativo_bonificable_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_remunerativo_bonificable_service.html");
          break;
        case "doc_backend_bussiness_logic_sancion_disciplinaria_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_sancion_disciplinaria_service.html");
          break;
        case "doc_backend_bussiness_logic_sexo_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_sexo_service.html");
          break;
        case "doc_backend_bussiness_logic_subjurisdicciones_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_subjurisdicciones_service.html");
          break;
        case "doc_backend_bussiness_logic_subprogramas_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_subprogramas_service.html");
          break;
        case "doc_backend_bussiness_logic_tipo_acceso_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_tipo_acceso_service.html");
          break;
        case "doc_backend_bussiness_logic_tipo_concepto_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_tipo_concepto_service.html");
          break;
        case "doc_backend_bussiness_logic_tipo_documento_service":
            loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_tipo_documento_service.html");
            break;
        case "doc_backend_bussiness_logic_tipo_horario_service":
            loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_tipo_horario_service.html");
            break;
        case "doc_backend_bussiness_logic_tipo_licencia_service":
            loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_tipo_licencia_service.html");
            break;
        case "doc_backend_bussiness_logic_tipo_planta_service":
            loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_tipo_planta_service.html");
            break;
        case "doc_backend_bussiness_logic_tipo_unidad_fisica_service":
            loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_tipo_unidad_fisica_service.html");
            break;
        case "doc_backend_bussiness_logic_ubicacion_geografica_service":
            loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_ubicacion_geografica_service.html");
            break;
        case "doc_backend_bussiness_logic_unidad_organizativa_service":
            loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_unidad_organizativa_service.html");
            break;
        case "doc_backend_bussiness_logic_users_service":
            loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_users_service.html");
            break;
        case "doc_backend_bussiness_logic_validacion_lote_service":
            loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_validacion_lote_service.html");
            break;
        case "doc_backend_bussiness_logic_verificar_externos_service":
            loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_verificar_externos_service.html");
            break;
        case "doc_database_schema":
            loadDashboardView(window.VIEWS_PATH + "/doc_database_schema.html");
            break;
        case "doc_db_gls_usuarios":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_usuarios.html");
            break;
        case "doc_db_gls_roles":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_roles.html");
            break;
        case "doc_db_gls_ch":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_ch.html");
            break;
        case "doc_db_gls_dp":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_dp.html");
            break;
        case "doc_db_gls_lh1":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_lh1.html");
            break;
        case "doc_db_gls_lh2":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_lh2.html");
            break;
        case "doc_db_gls_parametros":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_parametros.html");
            break;
        case "doc_db_gls_instituciones":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_instituciones.html");
            break;
        case "doc_db_gls_jurisdicciones":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_jurisdicciones.html");
            break;
        case "doc_db_gls_sub_jurisdicciones":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_sub_jurisdicciones.html");
            break;
        case "doc_db_gls_entidades":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_entidades.html");
            break;
        case "doc_db_gls_programas":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_programas.html");
            break;
        case "doc_db_gls_subprogramas":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_subprogramas.html");
            break;
        case "doc_db_gls_proyectos":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_proyectos.html");
            break;
        case "doc_db_gls_actividades":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_actividades.html");
            break;
        case "doc_db_gls_unidad_organizativa":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_unidad_organizativa.html");
            break;
        case "doc_db_gls_ubicacion_geografica":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_ubicacion_geografica.html");
            break;
        case "doc_db_gls_permisos_usuarios":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_permisos_usuarios.html");
            break;
        case "doc_db_gls_modulos":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_modulos.html");
            break;
        case "doc_db_gls_tipo_documento":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_tipo_documento.html");
            break;
        case "doc_db_gls_identidad_genero":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_identidad_genero.html");
            break;
        case "doc_db_gls_estado_civil":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_estado_civil.html");
            break;
        case "doc_db_gls_nacionalidad":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_nacionalidad.html");
            break;
        case "doc_db_gls_codigo_discapacidad":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_codigo_discapacidad.html");
            break;
        case "doc_db_gls_codigo_disciplina_estudio":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_codigo_disciplina_estudio.html");
            break;
        case "doc_db_gls_tipo_concepto":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_tipo_concepto.html");
            break;
        case "doc_db_gls_remunerativo_bonificable":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_remunerativo_bonificable.html");
            break;
        case "doc_db_gls_tipo_unidad_fisica":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_tipo_unidad_fisica.html");
            break;
        case "doc_db_gls_fuente_financiamiento":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_fuente_financiamiento.html");
            break;
        case "doc_db_gls_tipo_planta":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_tipo_planta.html");
            break;
        case "doc_db_gls_tipo_licencia":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_tipo_licencia.html");
            break;
        case "doc_db_gls_sancion_disciplinaria":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_sancion_disciplinaria.html");
            break;
        case "doc_db_gls_tipo_horario":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_tipo_horario.html");
            break;
        case "doc_db_gls_tipo_acceso":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_tipo_acceso.html");
            break;
        case "doc_db_gls_sexo":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_sexo.html");
            break;
        case "doc_db_gls_nivel_educativo":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_nivel_educativo.html");
            break;



        default:
          console.warn("⚠️ ID de navegación no mapeado en el enrutador central.");
          break;
      }
    }
  });

  // ✅ Exportar globalmente para que otros scripts lo usen
  window.loadDashboardView = loadDashboardView;
});


// ====================================================================================================================== //
// Función para exportar la vista actual a PDF
// ====================================================================================================================== //
window.exportarAPDF = function(nombreArchivo = 'Documentacion_Tecnica') {
    // Apuntamos al contenedor principal de la documentación (usualmente la clase jumbotron)
    const elemento = document.querySelector('#dashboard_views .jumbotron');

    if (!elemento) {
        alert("No se encontró el contenido para exportar.");
        return;
    }

    // Ocultamos temporalmente el botón de "Volver" y "Descargar" para que no salgan en el PDF
    const botones = elemento.querySelectorAll('button');
    botones.forEach(btn => btn.style.display = 'none');

    // Configuración del PDF
    const opciones = {
        margin:       10,
        filename:     `${nombreArchivo}.pdf`,
        image:        { type: 'jpeg', quality: 1 },
        html2canvas:  { scale: 4, useCORS: true },
        jsPDF:        { unit: 'mm', format: 'a4', orientation: 'portrait' }
    };

    // Generar el PDF
    html2pdf().set(opciones).from(elemento).save().then(() => {
        // Restaurar los botones una vez que se descargó el PDF
        botones.forEach(btn => btn.style.display = '');
    });
};

// ====================================================================================================================== //

/*
 * Función que carga el formulario de datos de usuario
 * Recibe @userName como parámetro para luego pedir los datos a la base de datos de dicho usuario
 * Los campos Nombre | Email | Rol en el formulario estan bloqueados ya que no se pueden modificar.
 * Los campos habilitados son Password_1 | Password_2 para que el usuario pueda cambiar su password
 */
async function loadDataUserForm(userName) {
  console.log("📄 Cargando formulario de Datos de Usuario para:", userName);
  const container = document.getElementById("dashboard_views");

  try {
    const responseView = await fetch(window.VIEWS_PATH + "/user_data.html");
    if (!responseView.ok) throw new Error(`HTTP ${responseView.status}`);
    container.innerHTML = await responseView.text();

    // LLAMADA AL BACKEND
    const responseData = await fetch(window.API_BASE_URL +"/users/get-user", {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: `user_name=${encodeURIComponent(userName)}`
    });

    if (!responseData.ok) throw new Error("No se pudo obtener la data del usuario");

    const text = await responseData.text();
    console.log("Contenido crudo del backend:", text);
    const data = JSON.parse(text);
    console.log("📦 Datos recibidos para el usuario:", data);

    setTimeout(() => {
      if (data) {
        if (document.getElementById("id")) document.getElementById("id").value = data.id || "";
        if (document.getElementById("nombre")) document.getElementById("nombre").value = data.nombre || "";
        if (document.getElementById("email")) document.getElementById("email").value = data.email || "";
        if (document.getElementById("rol")) document.getElementById("rol").value = data.rol || "";
        console.log("✅ Campos del formulario cargados");
      }
    }, 150);

    // 5. Configurar el evento Submit del formulario de edición
    const form = document.getElementById("user_data_form");

    if (form) {
      form.onsubmit = async (e) => {
        e.preventDefault();

        const formData = new URLSearchParams(new FormData(form));

        // Aseguramos nombres de campos para el backend C
        formData.set("email", document.getElementById("email").value);
        formData.set("password_1", document.getElementById("password_1").value);
        formData.set("password_2", document.getElementById("password_2").value);

        const res = await fetch(window.API_BASE_URL +"/password", {
          method: "POST",
          headers: { "Content-Type": "application/x-www-form-urlencoded" },
          body: formData.toString()
        });

        const result = await res.json();
        console.log("✅ Respuesta del backend:", result);

        const msgDiv = document.getElementById('msg-update-data-user');

        if (result.status === "ok") {
          msgDiv.innerHTML = `<br><div class="alert alert-success alert-dismissible">
                                <a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
                                <p align=center><span class="glyphicon glyphicon-ok"></span> ${result.message}</p></div>`;
          setTimeout(() => { $(".close").click(); }, 3000);
          setTimeout(() => { loadDashboardView(window.VIEWS_PATH + "/home.html"); }, 4000);
        } else {
          msgDiv.innerHTML = `<br><div class="alert alert-danger alert-dismissible">
                                <a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
                                <p align=center><span class="glyphicon glyphicon-warning-sign"></span> ${result.message}</p></div>`;
          setTimeout(() => { $(".close").click(); }, 3000);
        }
      };
    }

  } catch (error) {
    console.error("💥 Error en loadDataUserForm:", error);
  }
}

window.loadDataUserForm = loadDataUserForm;
