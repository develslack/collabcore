console.clear(); // SE LIMPIA LA CONSOLA DEL NAVEGADOR ALÑ INGRESAR AL MODULO USUARIOS
console.log("✅ Usuarios...");

// ===================================================================================================================== //

/*
 *  FUNCION ASIMETRICA QUE LISTA LOS USUARIOS
 *  ENVIA PETICION AL BACKEND EN LA URL http://localhost:3000/users/list
 *  LOS DATOS OBTENIDOS EN JSON SERÁN MOSTRADOS EN EL ARCHIVO usuarios.html
 *  POR MEDIO DEL BOTON EDITAR ROL SE PODRÁ CAMBIAR EL ROL DEL USUARIO SELECCIONADO
 */

(async () => {
  try {
    const response = await fetch(window.API_BASE_URL +"/users/list");
    const usuarios = await response.json();

    console.log("📋 Iniciando carga de usuarios...");
    console.log("✅ Usuarios recibidos satisfactoriamente");

    const tableBody = document.getElementById("usersTableBody");
    const alertInfo = document.getElementById("alert-info");
    tableBody.innerHTML = "";

    let count = 0;

    usuarios.forEach((u) => {
      const row = document.createElement("tr");
      row.innerHTML = `
        <tr>
        <td class="text-center">${u.nombre}</td>
        <td class="text-center">${u.email}</td>
        <td class="text-center">${u.rol}</td>
        <td class="text-center">
          <button class="btn btn-warning btn-sm" id="editar_rol_usuario" data-id="${u.id}">
            <span class="glyphicon glyphicon-edit"></span> Editar Rol</button>
        </td></tr>`;

      tableBody.appendChild(row);
      count++;
    });

    alertInfo.innerHTML = `<div class="alert alert-info">
                            <span class="glyphicon glyphicon-option-vertical" aria-hidden="true"></span> <strong>Cantidad de Registros: </strong> ${count}
                           </div><hr>`;

    console.log(`📊 Total de usuarios cargados: ${count}`);

    // Si usás DataTables
    if ($.fn.DataTable.isDataTable("#usuariosTable")) {
      $("#usuariosTable").DataTable().destroy();
    }


// ===================================================================================================================== //

/*
 * FUNCION SOBRE EL BOTÓN EDITAR ROL
 * AL PRESIONARLO SE CAPTURA EL VALOR DEL ID DE USUARIO
 * UNA VEZ CAPTURADO SE CARGA EL ROL DE USUARIO EN EL SELECT PARA REALIZAR LA MODIFICACIÓN DEL MISMO
 * LLAMANDO A LA FUNCION loadUserRolForm LA CUAL RECIBE COMO PARAMETRO userId
 */

document.addEventListener("click", async (e) => {

  console.log("🧭 Escuchando eventos de submit global...");
  const btn = e.target.closest("#editar_rol_usuario");
  if (!btn) return; // ignorar otros clicks

  e.preventDefault();
  const userId = btn.dataset.id;
  console.log("🧩 Editar rol para usuario ID:", userId);

  // Cargar la vista del formulario de rol
  await loadDashboardView(window.VIEWS_PATH + "/rol_usuario.html");

  // Una vez cargado el HTML, ejecutar la función que busca los datos del usuario
  loadUserRolForm(userId);
});

// ===================================================================================================================== //


async function loadUserRolForm(userId) {

    console.log("🔍 Iniciando configuración de edición para ID:", userId);

  try {
    // 1. Obtener el rol actual del usuario desde el backend
    const response = await fetch(window.API_BASE_URL +"/users/get-role", {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: `id=${userId}`
    });

    if (!response.ok) throw new Error("Error al obtener el rol del usuario");
    const data = await response.json();
    console.log("📦 Rol actual detectado:", data.rol); // El backend debería devolver el ID del rol ahora

    // 2. Poblamos el SELECT con la nueva función dinámica
    // Pasamos 'rol' como ID del select y data.rol_id como el valor a pre-seleccionar
    await loadRoles("rol", data.rol_id || data.rol);

    const form = document.getElementById("form-user-rol");
    if (form) {
      document.getElementById("id_usuario").value = userId;

      form.addEventListener("submit", async (e) => {
        e.preventDefault();
        const formData = new FormData(form);
        const id = formData.get("id");
        const rolId = formData.get("rol"); // Ahora capturamos el ID del rol

        try {
          const res = await fetch(window.API_BASE_URL +"/users/update-role", {
            method: "POST",
            headers: { "Content-Type": "application/x-www-form-urlencoded" },
            body: `id=${encodeURIComponent(id)}&rol=${encodeURIComponent(rolId)}`
          });

          const result = await res.json();

          if (result.status === "ok") {
              var mensaje = `<br><div class="alert alert-success alert-dismissible">
                                    <a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
                                        <p align=center><span class="glyphicon glyphicon-ok" aria-hidden="true"></span> ${result.message}</p></div>`;
              document.getElementById('msg-update-rol').innerHTML = mensaje;
              setTimeout(function() { $(".close").click(); }, 3000);
              setTimeout(function() { loadDashboardView(window.VIEWS_PATH + "/usuarios.html"); }, 4000);

            } else {
              var mensaje = `<br><div class="alert alert-danger alert-dismissible">
                                    <a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
                                        <p align=center><span class="glyphicon glyphicon-warning-sign" aria-hidden="true"></span> ${result.message}</p></div>`;
              document.getElementById('msg-update-rol').innerHTML = mensaje;
              setTimeout(function() { $(".close").click(); }, 3000);

            }
        } catch (err) {
          console.error("Error al actualizar:", err);
        }
      });
    }
  } catch (err) {
    console.error("💥 Error en el flujo de roles:", err);
  }
}

/*
// --- FUNCION PARA CARGAR EL FORMULARIO DE RELLENADO DEL FORMULARIO DATOS DE USUARIO Y ACTUALIZACIÓN DE PASSWORD DESDE EL DASHBORAD ---

async function loadDataUserForm(userName) {
    console.log("📄 Cargando formulario de Datos de Usuario para:", userName);
    const container = document.getElementById("dashboard_views");

    try {
        // 1. Cargar la vista HTML
        const responseView = await fetch(window.VIEWS_PATH + "/user_data.html");
        if (!responseView.ok) throw new Error(`HTTP ${responseView.status}`);
        container.innerHTML = await responseView.text();


        // 3. Consultar los datos del usuario por POST
        const responseData = await fetch(window.API_BASE_URL +"/users/get-user", {
            method: "POST",
            headers: { "Content-Type": "application/x-www-form-urlencoded" },
            body: `user_name=${userName}`
        });

        if (!responseData.ok) throw new Error("No se pudo obtener la data del usuario");

        const data = await responseData.json();
        console.log("📦 Datos recibidos para el usuario:", data);

        // 4. Rellenar el formulario (CON RETARDO PARA ESPERAR AL DOM)
        setTimeout(() => {
            if (data) {
                const elId = document.getElementById("id");
                const elNombre = document.getElementById("nombre");
                const elEmail = document.getElementById("email");
                const elRol = document.getElementById("rol");

                // Solo asignamos si el elemento fue encontrado en el DOM
                if (elId) elId.value = data.id || "";
                if (elNombre) elNombre.value = data.nombre || "";
                if (elEmail) elEmail.value = data.email || "";
                if (elRol) elRol.value = data.rol || "";

                console.log("✅ Campos del formulario cargados");
            }
        }, 150); // 150ms es el tiempo ideal para asegurar el renderizado

        // 5. Configurar el evento Submit del formulario de edición
        const form = document.getElementById("user_data_form");
        if (form) {
            form.onsubmit = async (e) => { // Usamos onsubmit para evitar duplicidad de eventos
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
        var mensaje = `<br><div class="alert alert-danger alert-dismissible">
                                    <a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
                                        <p align=center><span class="glyphicon glyphicon-warning-sign" aria-hidden="true"></span> ERROR! Al enviar datos...${error}</p></div>`;
              document.getElementById('msg-update-data-user').innerHTML = mensaje;
              setTimeout(function() { $(".close").click(); }, 3000);
    }
} // FIN DE LA FUNCION
*/
// ===================================================================================================================== //

/*
 * FUNCION QUE ORGANIZA LA TABLA DE USUARIOS
 * UTILIZA DATA-TABLES PARA ORGANIZAR LOS DATOS
 */

    // ESTRUCTURA TABLE
 $(document).ready(function(){
      $('#usuariosTable').DataTable({
        "order": [[0, "asc"]],
        "responsive":     true,
        "scrollY":        "300px",
        "scrollX":        true,
        "scrollCollapse": true,
        "paging":         true,
        "deferRender": true,
        "retrieve": true,
        "dom": '<"row"<"col-sm-12"l>><"row"<"col-sm-12"Bf>>rt<"row"<"col-sm-12"ip>>',
        buttons: [
            {
                extend: 'excel',
                text: '<i class="glyphicon glyphicon-list-alt"></i> Export Excel',
                titleAttr: 'Export Excel',
                messageTop: 'Listado de Usuarios',
                exportOptions: { columns: ':visible',}
            },
            {
                extend: 'csv',
                text: '<i class="glyphicon glyphicon-align-justify"></i> Export CSV',
                titleAttr: 'Exportar a CSV',
                messageTop: 'Listado de Usuarios',
                exportOptions: { columns: ':visible',}
            },
            {
                extend: 'pdf',
                text: '<i class="glyphicon glyphicon-file"></i> Export PDF',
                titleAttr: 'Exportar a PDF',
                messageTop: 'Listado de Usuarios',
                exportOptions: { columns: ':visible',}
            },
            {
                extend: 'print',
                text: '<i class="glyphicon glyphicon-print"></i> Imprimir',
                customize: function ( win ) {
                    $(win.document.body)
                        .css( 'font-size', '8pt' );


                    $(win.document.body).find( 'table' )
                        .addClass( 'compact' )
                        .css( 'font-size', 'inherit' );
                },
                messageTop: 'Listado de Usuarios',
                autoPrint: false,
                exportOptions: {
                    columns: ':visible',
                }

            },
            'colvis'
        ],
        columnDefs: [ {
            targets: -1,
            visible: true
        } ],
        "fixedColumns": true,
      "language":{
        "lengthMenu": "Mostrar _MENU_ registros por pagina",
        "info": "Mostrando pagina _PAGE_ de _PAGES_",
        "infoEmpty": "No hay registros disponibles",
        "infoFiltered": "(filtrada de _MAX_ registros)",
        "loadingRecords": "Cargando...",
        "processing":     "Procesando...",
        "search": "Buscar:",
        "zeroRecords":    "No se encontraron registros coincidentes",
        "paginate": {
          "next":       "Siguiente",
          "previous":   "Anterior"
        },
      }
    });
});


  } catch (error) {
    console.error("💥 Error al obtener usuarios:", error);
  }
})();

// Función para poblar el select de roles desde el backend
async function loadRoles(selectId, currentRoleId = null) {

    const select = document.getElementById(selectId);
    if (!select) return;

    try {
        const response = await fetch(window.API_BASE_URL +"/roles/list");
        const roles = await response.json();

        select.innerHTML = '<option value="" disabled>Seleccione un Rol</option>';

        roles.forEach(rol => {
            const option = document.createElement("option");
            option.value = rol.id;
            option.textContent = rol.descripcion;

            // Comparación numérica o de string para pre-seleccionar
            if (currentRoleId && rol.id == currentRoleId) {
                option.selected = true;
            }
            select.appendChild(option);
        });
        return true; // Indicamos que terminó bien
    } catch (error) {
        console.error("❌ Error al cargar roles:", error);
        return false;
    }
}




