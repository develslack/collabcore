console.clear();
console.log("✅ Módulo Grupos Iniciado de forma nativa.");

// CORRECCIÓN: Usamos el objeto window para evitar el SyntaxError de redeteclaración con let
window.dTable = window.dTable || null;

(async () => {
  try {
    // ================================================================================================================= //
    // 1. CARGA DE LA TABLA MAESTRA DESDE EL BACKEND EN C
    // ================================================================================================================= //
    const response = await fetch(window.API_BASE_URL +"/grupos/list");
    const grupos = await response.json();

    const tableBody = document.getElementById("gruposTableBody");
    const alertInfo = document.getElementById("alert-info-grupos");
    tableBody.innerHTML = "";

    let count = 0;

    grupos.forEach((data) => {
      const row = document.createElement("tr");

      // 1. Iniciamos la variable con la primera parte
        let html = `
            <td class="text-center">${data.nombre}</td>
            <td class="text-center">${data.nombre_directorio}</td>
            <td class="text-center">${data.fecha_creacion}</td>
        `;

        // 2. Concatenamos (+=) la condición correspondiente
        if (data.id_creador == 1) {
            html += `<td class="text-center"><span class="label label-danger">${data.nombre_creador}</span></td>`;
        } else {
            html += `<td class="text-center"><span class="label label-warning">${data.nombre_creador}</span></td>`;
        }

        // 3. Concatenamos (+=) el resto de los botones
        html += `
            <td class="text-center">
            <button class="btn btn-warning btn-sm btn-editar-grupo"
                    data-id="${data.id}"
                    data-nombre="${data.nombre}"
                    data-id_creador="${data.id_creador}">
                <span class="glyphicon glyphicon-edit"></span> Editar
            </button>
            <button class="btn btn-primary btn-sm btn-integrantes-grupo"
                    data-id="${data.id}">
                <span class="glyphicon glyphicon-th-large"></span> Integrantes
            </button>
            </td>
        `;

        // 4. Asignamos todo el string final al elemento
        row.innerHTML = html;

      tableBody.appendChild(row);
      count++;
    });

    if (alertInfo) {
        alertInfo.innerHTML = `<div class="alert alert-info">
                                <span class="glyphicon glyphicon-option-vertical" aria-hidden="true"></span> <strong>Cantidad de Registros: </strong> ${count}
                               </div><hr>`;
    }

    if ($.fn.DataTable.isDataTable("#gruposTable")) {
      $("#gruposTable").DataTable().destroy();
    }

    dTable = $('#gruposTable').DataTable({
        "order": [[0, "asc"]],
        "responsive":     true,
        "scrollY":        "300px",
        "scrollX":        true,
        "scrollCollapse": true,
        "paging":         true,
        "deferRender":    true,
        "retrieve":       true,
        "dom": '<"row"<"col-sm-12"l>><"row"<"col-sm-12"Bf>>rt<"row"<"col-sm-12"ip>>',
        buttons: [
            { extend: 'excel', text: '<i class="glyphicon glyphicon-list-alt"></i> Export Excel', exportOptions: { columns: ':visible'} },
            { extend: 'csv', text: '<i class="glyphicon glyphicon-align-justify"></i> Export CSV', exportOptions: { columns: ':visible'} },
            { extend: 'pdf', text: '<i class="glyphicon glyphicon-file"></i> Export PDF', exportOptions: { columns: ':visible'} },
            {
                extend: 'print',
                text: '<i class="glyphicon glyphicon-print"></i> Imprimir',
                customize: function ( win ) {
                    $(win.document.body).css( 'font-size', '8pt' );
                    $(win.document.body).find( 'table' ).addClass( 'compact' ).css( 'font-size', 'inherit' );
                },
                autoPrint: false,
                exportOptions: { columns: ':visible' }
            },
            'colvis'
        ],
        "language": {
            "lengthMenu": "Mostrar _MENU_ registros por pagina",
            "info": "Mostrando pagina _PAGE_ de _PAGES_",
            "search": "Buscar:",
            "zeroRecords":    "No se encontraron registros coincidentes",
            "paginate": { "next": "Siguiente", "previous": "Anterior" }
        }
    });

    // ================================================================================================================= //
    // 2. PREPARACIÓN DEL MODAL USANDO EL SISTEMA DEL DASHBOARD
    // ================================================================================================================= //
    // Usamos el fetch nativo del servidor para rellenar el div de abajo sin pisar el dashboard central
    const contenedorModal = document.getElementById("contenedor-modal-maestro-grupos");
    if (contenedorModal) {
        const respModal = await fetch(window.VIEWS_PATH + "/grupos/nuevo_grupo.html");
        contenedorModal.innerHTML = await respModal.text();
    }

    // ================================================================================================================= //
    // 3. CAPTURA SEGURO DE EVENTOS (DELEGACIÓN DE EVENTOS JQUERY)
    // ================================================================================================================= //

    // Evento Añadir (Alta)
    $(document).off("click", "#add-grupo-form").on("click", "#add-grupo-form", function(e) {
        e.preventDefault();
        prepararYMostrarModal("alta");
    });

    // Evento Editar (Fila de la tabla)
    $(document).off("click", ".btn-editar-grupo").on("click", ".btn-editar-grupo", function(e) {
        e.preventDefault();
        const datos = {
            id: $(this).attr("data-id"),
            nombre: $(this).attr("data-nombre"),
            id_creador: $(this).attr("data-id_creador")
        };
        prepararYMostrarModal("edicion", datos);
    });

  } catch (error) {
    console.error("💥 Error general en el módulo Grupos:", error);
  }
})();

// ===================================================================================================================== //
// FUNCIÓN QUE CONSTRUIE EL FORMULARIO INTERNO Y MUESTRA EL MODAL FLOTANTE
// ===================================================================================================================== //
async function prepararYMostrarModal(modo, datos = null) {
    const $modal = $("#myModal-grupos");
    if ($modal.length === 0) return;

    // Seteamos título del modal flotante
    $modal.find(".modal-title").text(modo === "alta" ? "Añadir Grupo" : "Editar Grupo");

    // Seteamos campos en el body vacío
    $modal.find(".modal-body").html(`

        <form id="form-modal-grupo">
            <input type="hidden" id="modal-grupo-id" value="${datos ? datos.id : ''}">
            <div class="form-group">
                <label><span class="label label-default">Nombre Grupo:</span></label>
                <input type="text" class="form-control" id="modal-grupo-nombre" value="${datos ? datos.nombre : ''}" required>
            </div>
            <div class="form-group">
                <label for="modal-grupo-id_creador"><span class="label label-default">Creador</span></label>
                <select class="form-control" id="modal-grupo-id_creador" required style="font-weight: bold;">
                  <option value="">Cargando...</option>
                </select>
              </div>

            <button type="submit" id="btn-submit-oculto" style="display:none;"></button>
        </form>
    `);

    // Seteamos barra de estado y botones en el footer vacío
    $modal.find(".modal-footer").html(`
        <div id="modal-status-message" style="margin-bottom: 10px; text-align: left;"></div>
        <div class="text-right">
            <button type="button" class="btn btn-danger" data-dismiss="modal">
                <span class="glyphicon glyphicon-remove-circle" aria-hidden="true"></span> Cerrar</button>
            <button type="button" class="btn btn-success" id="btn-modal-guardar">
                <span class="glyphicon glyphicon-ok" aria-hidden="true"></span> Guardar</button>
        </div>
    `);

    // CARGAMOS EL COMBRO USUARIOS
    await cargarUsuarios(datos ? datos.id_creador : "");

    // Hacemos que aparezca de forma flotante sobre dashboard.html
    $modal.modal("show");

    // Vinculamos el botón verde del footer con la validación del form
    document.getElementById("btn-modal-guardar").onclick = () => {
        document.getElementById("btn-submit-oculto").click();
    };

    // Procesamiento del submit al backend en C
    document.getElementById("form-modal-grupo").onsubmit = async (e) => {
        e.preventDefault();

        const statusContainer = document.getElementById("modal-status-message");
        const id = document.getElementById("modal-grupo-id").value;
        const payload = {
            nombre: document.getElementById("modal-grupo-nombre").value,
            id_creador: document.getElementById("modal-grupo-id_creador").value
        };


        if(id == ""){

            statusContainer.innerHTML = `<span class="text-info">⏳ Procesando solicitud...</span>`;

            try {
                const enviorresp = await fetch(window.API_BASE_URL +"/grupos/add", {
                    method: "POST",
                    headers: { "Content-Type": "application/x-www-form-urlencoded" },
                    body: `nombre=${encodeURIComponent(payload.nombre)}&id_creador=${encodeURIComponent(payload.id_creador)}`
                });

                const data = await enviorresp.json();

                if (enviorresp.ok && data.status === "ok") {
                    statusContainer.innerHTML = `<span class="text-success"><strong>✅ ¡Éxito!</strong> ${data.message}.</span>`;

                    // Forzamos la actualización de la vista actual llamando al dashboard de forma nativa
                    setTimeout(() => {
                        $modal.modal("hide");
                        window.loadDashboardView(window.VIEWS_PATH + "/grupos/grupos.html"); // <--- RECARGA ASÍNCRONA OFICIAL DEL DASHBOARD
                    }, 1200);
                } else {
                    statusContainer.innerHTML = `<span class="text-danger"><strong>❌ Error:</strong> ${data.message}.</span>`;
                }
            }
            catch (err) {
                console.error(err);
                statusContainer.innerHTML = `<span class="text-danger"><strong>💥 Error:</strong> ${data.message}.</span>`;
        }

        }
        if(id != ""){

            statusContainer.innerHTML = `<span class="text-info">⏳ Procesando solicitud...</span>`;

            try {
                const enviorresp = await fetch(window.API_BASE_URL +"/grupos/edit", {
                    method: "POST",
                    headers: { "Content-Type": "application/x-www-form-urlencoded" },
                    body: `id=${encodeURIComponent(id)}&nombre=${encodeURIComponent(payload.nombre)}&id_creador=${encodeURIComponent(payload.id_creador)}`
                });

                const data = await enviorresp.json();

                if (enviorresp.ok && data.status === "ok") {
                    statusContainer.innerHTML = `<span class="text-success"><strong>✅ ¡Éxito!</strong> ${data.message}.</span>`;

                    // Forzamos la actualización de la vista actual llamando al dashboard de forma nativa
                    setTimeout(() => {
                        $modal.modal("hide");
                        window.loadDashboardView(window.VIEWS_PATH + "/grupos/grupos.html"); // <--- RECARGA ASÍNCRONA OFICIAL DEL DASHBOARD
                    }, 1200);
                } else {
                    statusContainer.innerHTML = `<span class="text-danger"><strong>❌ Error:</strong> ${data.message}.</span>`;
                }
            }
            catch (err) {
                console.error(err);
                statusContainer.innerHTML = `<span class="text-danger"><strong>💥 Error:</strong> ${data.message}.</span>`;
        }

        }

    };
}

async function cargarUsuarios(valorSeleccionado) {
    const select = document.getElementById("modal-grupo-id_creador");
    if (!select) return;

    try {
        const resp = await fetch(window.API_BASE_URL +"/users/list");
        const lista = await resp.json();

        select.innerHTML = `<option value="">-- Seleccionar --</option>`;
        lista.forEach(item => {
            const id = item.id ? item.id : item.id;
            const nombre = item.nombre ? item.nombre.trim() : "";
            const opt = document.createElement("option");
            opt.value = id;
            opt.textContent = `${nombre}`;
            if (id == valorSeleccionado) opt.selected = true;
            select.appendChild(opt);
        });
    } catch (err) {
        console.error("Error al cargar usuarios:", err);
        select.innerHTML = `<option value="">-- Seleccionar --</option>`;
    }
}
