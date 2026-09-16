DELIMITER //

-- ====================================================
-- STORED PROCEDURE: Agregar Integrante
-- ====================================================
DROP PROCEDURE IF EXISTS sp_insertar_miembro //

CREATE PROCEDURE sp_insertar_miembro(
    IN p_id_grupo INT,
    IN p_id_usuario INT,
    IN p_es_coordinador TINYINT
)
BEGIN
    -- Insertamos el registro.
    -- La fecha_ingreso se puede forzar con CURRENT_TIMESTAMP para mayor seguridad,
    -- y fecha_baja queda implícitamente en NULL indicando que está ACTIVO.
    INSERT INTO cc_miembros_grupo (id_grupo, id_usuario, es_coordinador, fecha_ingreso)
    VALUES (p_id_grupo, p_id_usuario, p_es_coordinador, CURRENT_TIMESTAMP);
END //


-- ====================================================
-- STORED PROCEDURE: Dar de Baja Integrante (Soft Delete)
-- ====================================================
DROP PROCEDURE IF EXISTS sp_baja_miembro //

CREATE PROCEDURE sp_baja_miembro(
    IN p_id_grupo INT,
    IN p_id_usuario INT
)
BEGIN
    -- En lugar de hacer un DELETE físico, le estampamos la fecha y hora actual de salida.
    -- La condición "fecha_baja IS NULL" asegura que solo afectemos el registro activo actual,
    -- en caso de que el usuario haya entrado y salido del mismo grupo varias veces en el pasado.
    UPDATE cc_miembros_grupo
    SET fecha_baja = CURRENT_TIMESTAMP
    WHERE id_grupo = p_id_grupo
      AND id_usuario = p_id_usuario
      AND fecha_baja IS NULL;
END //

DELIMITER ;
