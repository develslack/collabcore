DELIMITER //

-- ====================================================
-- STORED PROCEDURE: Insertar Grupo
-- ====================================================
DROP PROCEDURE IF EXISTS sp_insertar_grupo //

CREATE PROCEDURE sp_insertar_grupo(
    IN p_nombre VARCHAR(100),
    IN p_id_creador INT
)
BEGIN
    DECLARE v_nuevo_id INT;
    DECLARE v_nombre_dir VARCHAR(100);

    -- 1. Insertamos el grupo (nombre_directorio quedará en NULL momentáneamente)
    INSERT INTO cc_grupos (nombre, id_creador)
    VALUES (p_nombre, p_id_creador);

    -- 2. Capturamos el ID autogenerado por MySQL/MariaDB
    SET v_nuevo_id = LAST_INSERT_ID();

    -- 3. Generamos el nombre del directorio (ej. "grupo_14")
    SET v_nombre_dir = CONCAT('grupo_', v_nuevo_id);

    -- 4. Actualizamos el registro para asentar el directorio
    UPDATE cc_grupos
    SET nombre_directorio = v_nombre_dir
    WHERE id = v_nuevo_id;

    -- 5. Devolvemos el ID y el nombre del directorio a la aplicación en C
    SELECT v_nuevo_id AS nuevo_id, v_nombre_dir AS nombre_directorio;
END //


-- ====================================================
-- STORED PROCEDURE: Editar Grupo
-- ====================================================
DROP PROCEDURE IF EXISTS sp_editar_grupo //

CREATE PROCEDURE sp_editar_grupo(
    IN p_id INT,
    IN p_nombre VARCHAR(100)
)
BEGIN
    -- Solo permitimos editar el nombre, el id_creador y el directorio son inmutables
    UPDATE cc_grupos
    SET nombre = p_nombre
    WHERE id = p_id;
END //

DELIMITER ;
