console.clear(); // limpiamos la consola al ingresar al moduio de roles

// ===================================================================================================================== //
/*
 * FUNCION QUE CARGA EL LISTADO DE ROLES ENVIANDO CONSULTA AL BACKEND CON URL http://localhost:3000/roles/list
 * SOLO TENEMOS EL BOTON EDITAR
 *
 */
// ===================================================================================================================== //
(async () => {
  try {
    const response = await fetch(window.API_BASE_URL +"/roles/list");
    const roles = await response.json();
    console.clear();
    console.log("✅ Roles...");
    console.log("📋 Iniciando carga de Roles...");
    console.log("✅ Roles recibidos satisfactoriamente");

    const tableBody = document.getElementById("rolesTableBody");
    const alertInfo = document.getElementById("alert-info-roles");
    tableBody.innerHTML = "";

    let count = 0;

    roles.forEach((rol) => {
      const row = document.createElement("tr");
      row.innerHTML = `
        <tr>
        <td class="text-center">${rol.descripcion}</td>
        <td class="text-center">
          <button class="btn btn-primary btn-sm" id="editar_rol" data-id="${rol.id}">
            <span class="glyphicon glyphicon-edit"></span> Editar</button>
        </td></tr>`;

      tableBody.appendChild(row);
      count++;
    });

    alertInfo.innerHTML = `<div class="alert alert-info">
                            <span class="glyphicon glyphicon-option-vertical" aria-hidden="true"></span> <strong>Cantidad de Registros: </strong> ${count}
                           </div><hr>`;

    console.log(`📊 Total de roles cargados: ${count}`);

    // Si usás DataTables
    if ($.fn.DataTable.isDataTable("#rolesTable")) {
      $("#rolesTable").DataTable().destroy();
    }


// ===================================================================================================================== //

console.log("🦷 [roles.js] Módulo roles cargado.");

// ===================================================================================================================== //

// ===================================================================================================================== //
// Escuchar el SUBMIT del formulario de alta
// ===================================================================================================================== //
document.addEventListener("submit", async (e) => {
    if (e.target && e.target.id === "roles_add_form") {
        e.preventDefault(); // Detiene la recarga y la redirección al home
        e.stopImmediatePropagation();

        const formData = new FormData(e.target);
        const descripcion = formData.get("descripcion");

        console.log("📨 Enviando nuevo rol a /roles/add:", descripcion.toLowerCase());

        try {
            const res = await fetch(window.API_BASE_URL +"/roles/add", { // Ruta confirmada por usuario
                method: "POST",
                headers: { "Content-Type": "application/x-www-form-urlencoded" },
                body: `descripcion=${encodeURIComponent(descripcion.toLowerCase())}`
            });

            const result = await res.json();
            const msgContainer = document.getElementById('msg-nuevo-rol');

            if (result.status === "ok") {
                if (msgContainer) {
                    msgContainer.innerHTML = `<br><div class="alert alert-success alert-dismissible">
                        <a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
                        <p align=center><span class="glyphicon glyphicon-ok"></span> ${result.message}</p></div>`;
                }
                // Redirección suave tras éxito
                setTimeout(function() { $(".close").click(); }, 3000);
                setTimeout(() => { loadDashboardView(window.VIEWS_PATH + "/roles/roles.html"); }, 2000);
            } else {
                if (msgContainer) {
                    msgContainer.innerHTML = `<br><div class="alert alert-danger alert-dismissible">
                        <a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
                        <p align=center><span class="glyphicon glyphicon-warning-sign"></span> ${result.message}</p></div>`;
                    setTimeout(function() { $(".close").click(); }, 3000);
                }
            }
        } catch (err) {
            console.error("Error al guardar rol:", err);
        }
    }
});



// ===================================================================================================================== //
// Captura el clic en el botón Editar de la tabla y espera el ckick en boton submit
// ===================================================================================================================== //
//
// 1. EVENTO DE CLIC EN LA TABLA (MOMENTO DE CARGA)
document.addEventListener("click", async (e) => {
    const btn = e.target.closest("#editar_rol");
    if (!btn) return;

    e.preventDefault();
    const rolId = btn.dataset.id;

    // Carga la vista física
    await loadDashboardView(window.VIEWS_PATH + "/roles/editar_rol.html");

    try {
        // Pide los datos al backend
        const response = await fetch(window.API_BASE_URL +"/roles/get", {
            method: "POST",
            headers: { "Content-Type": "application/x-www-form-urlencoded" },
            body: `id=${encodeURIComponent(rolId)}`
        });
        const data = await response.json();

        // Esperamos un instante a que el HTML se renderice
        setTimeout(() => {
            const form = document.getElementById("roles_edit_form");
            const inputId = document.getElementById("id");
            const inputDesc = document.getElementById("descripcion");

            if (form && inputId && inputDesc) {
                // Rellenamos los datos para que el usuario los vea
                inputId.value = data.id;
                inputDesc.value = data.descripcion;

                console.log("✍️ Formulario listo para editar. Esperando al usuario...");

                // 2. DEFINICIÓN DEL EVENTO SUBMIT (MOMENTO DE GUARDADO)
                // Esto NO se ejecuta ahora, solo se "programa" para cuando hagas click en Actualizar
                form.onsubmit = async (event) => {
                    event.preventDefault();
                    event.stopPropagation(); // 👈 BLOQUEA AL DASHBOARD.JS AQUÍ

                    const formData = new FormData(form);
                    const id = formData.get("id");
                    const descripcion = formData.get("descripcion");

                    console.log("📨 Enviando cambios...");

                    const res = await fetch(window.API_BASE_URL +"/roles/edit", {
                        method: "POST",
                        headers: { "Content-Type": "application/x-www-form-urlencoded" },
                        body: `id=${encodeURIComponent(id)}&descripcion=${encodeURIComponent(descripcion.toLowerCase())}`
                    });

                    const result = await res.json();

                    if (result.status === "ok") {
                        var mensaje = `<br><div class="alert alert-success alert-dismissible">
                                                <a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
                                                    <p align=center><span class="glyphicon glyphicon-ok" aria-hidden="true"></span> ${result.message}</p></div>`;
                        document.getElementById('msg-editar-rol').innerHTML = mensaje;
                        setTimeout(function() { $(".close").click(); }, 3000);
                        setTimeout(function() { loadDashboardView(window.VIEWS_PATH + "/roles/roles.html"); }, 4000);

                        } else {
                        var mensaje = `<br><div class="alert alert-danger alert-dismissible">
                                                <a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
                                                    <p align=center><span class="glyphicon glyphicon-warning-sign" aria-hidden="true"></span> ${result.message}</p></div>`;
                        document.getElementById('msg-editar-rol').innerHTML = mensaje;
                        setTimeout(function() { $(".close").click(); }, 3000);
                    }
                    return false;
                };
            }
        }, 200);
    } catch (error) {
        console.error("Error:", error);
    }
});



// ===================================================================================================================== //
/*
 * ESTRUCTURA DE LA TABLA ROLES UTILIZANDO DATA-TABLES
 *
 */
// ===================================================================================================================== //

 $(document).ready(function(){
      $('#rolesTable').DataTable({
        "order": [[0, "asc"]],
        "responsive":     true,
        "scrollY":        "300px",
        "scrollX":        true,
        "scrollCollapse": true,
        "paging":         true,
        "deferRender": true,
        "retrieve": true,
        "stateSave": true,
        "dom": '<"row"<"col-sm-12"l>><"row"<"col-sm-12"Bf>>rt<"row"<"col-sm-12"ip>>',

        buttons: [
            {
                extend: 'excel',
                text: '<i class="glyphicon glyphicon-list-alt"></i> Export Excel',
                titleAttr: 'Export Excel',
                messageTop: 'Listado de Roles',
                exportOptions: { columns: ':visible',}
            },
            {
                extend: 'csv',
                text: '<i class="glyphicon glyphicon-align-justify"></i> Export CSV',
                titleAttr: 'Exportar a CSV',
                messageTop: 'Listado de Roles',
                exportOptions: { columns: ':visible',}
            },
            {
                extend: 'pdf',
                text: '<i class="glyphicon glyphicon-file"></i> Export PDF',
                titleAttr: 'Exportar a PDF',
                messageTop: 'Listado de Roles',
                exportOptions: { columns: ':visible',}
            },
            {
                extend: 'print',
                text: '<i class="glyphicon glyphicon-print"></i> Imprimir',
                titleAttr: 'Imprimir tabla',
                customize: function ( win ) {
                    $(win.document.body)
                        .css( 'font-size', '8pt' );


                    $(win.document.body).find( 'table' )
                        .addClass( 'compact' )
                        .css( 'font-size', 'inherit' );
                },
                messageTop: 'Listado de Roles',
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
    console.error("💥 Error al obtener roles:", error);
  }
})();

// ===================================================================================================================== //
// FIN DE LALOGICA
// ===================================================================================================================== //
