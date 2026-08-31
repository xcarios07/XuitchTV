# XuitchTV v0.5.1 — First hardware test checklist

## Antes de iniciar

1. Generar `XuitchTV.nro` con `./scripts/release-switch.sh` o GitHub Actions.
2. Copiar el contenido de `dist/XuitchTV-SD.zip` a la raíz de la microSD.
3. Confirmar que existe `/switch/XuitchTV/XuitchTV.nro` y `/switch/XuitchTV/config.json`.
4. Mantener la playlist IPTV de Paraguay en `config.json` para la primera prueba.

## Inicio recomendado

Para reproducción multimedia estable, ejecutar Homebrew Menu mediante **title takeover / full RAM**. XuitchTV mostrará una advertencia si detecta Applet Mode.

## Secuencia MVP

1. Abrir XuitchTV.
2. Entrar a `IPTV Paraguay`.
3. Pulsar `Actualizar`.
4. Confirmar que se muestra una cantidad de canales mayor que cero.
5. Abrir un canal.
6. Confirmar los estados: `Abriendo stream...` → `Reproduciendo`.
7. Confirmar video y audio.
8. Probar `Pausa` / `Reanudar`.
9. Salir con `B` o `Salir`.
10. Probar un segundo canal.

## Si falla

Registrar exactamente:

- si XuitchTV abre o se cierra al iniciar;
- si la playlist carga y cuántos canales aparecen;
- nombre del canal probado;
- texto exacto del error mostrado por XuitchTV;
- si hay audio sin video o video sin audio;
- si se ejecutó en Applet Mode o full RAM;
- si la consola estaba en portátil o dock.

Con `nxlink`, usar `./scripts/deploy-nxlink.sh <IP_SWITCH>` para obtener logs de una ejecución de depuración.
