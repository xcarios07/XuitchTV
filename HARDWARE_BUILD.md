# XuitchTV v0.5.1 - First Hardware Build

Esta etapa tiene un objetivo: producir y probar `XuitchTV.nro` en Nintendo Switch.

## Dependencias Switch

La ruta de build sigue la misma familia de herramientas usada por clientes multimedia nativos actuales:

- devkitPro / devkitA64
- libnx
- Borealis (rama `wiliwili`)
- switch-curl
- switch-libmpv
- switch-glfw
- switch-libwebp

## Opcion A - Docker

```bash
./scripts/bootstrap-deps.sh
./scripts/build-switch-docker.sh
./scripts/package-sd.sh
```

Resultado esperado:

```text
build_switch/XuitchTV.nro
dist/XuitchTV-SD.zip
```

## Opcion B - devkitPro local

Instalar los paquetes requeridos y luego:

```bash
./scripts/preflight-switch.sh
./scripts/bootstrap-deps.sh
./scripts/build-switch.sh
./scripts/package-sd.sh
```

## Opcion C - GitHub Actions

El repositorio incluye `.github/workflows/build-switch.yml`. Al ejecutar manualmente el workflow **Build Nintendo Switch NRO**, se generan dos artifacts:

- `XuitchTV-nro`
- `XuitchTV-SD`

## Instalacion de prueba

Copiar la carpeta resultante a:

```text
sd:/switch/XuitchTV/
  XuitchTV.nro
  config.json
```

Para video con libmpv se recomienda iniciar Homebrew Menu con acceso completo a memoria (title takeover), no desde applet mode limitado.

## Prueba MVP

1. Abrir XuitchTV.
2. Entrar a **IPTV Paraguay**.
3. Pulsar **Actualizar**.
4. Seleccionar un canal.
5. Confirmar video + audio.
6. Probar pausa/reanudacion y volver.
7. Probar modo portatil y dock.

Si un canal falla, el reproductor v0.5.1 conserva el error reportado por libmpv para distinguir fallo de red/stream de un problema de render.
