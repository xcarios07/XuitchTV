# Mapa de port inicial

## Fuente analizada

APK suministrado: `Xuper.Hydra.HDR.4KP.apk`. El ZIP de GitHub aportado contiene solo un README, por lo que el APK es la fuente principal de cartografia estatica.

## Arquitectura Android recuperada

- Paquete: `com.xuper.netxxus`
- Entrada: `WelcomeActivity`
- Android TV / Leanback
- Modulos visibles: Home, Login, Live TV, VOD, Deportes, Cuenta, Descargas
- Player Android: IJKPlayer/FFmpeg y referencias a ExoPlayer
- Native ABI: arm64-v8a y armeabi-v7a

## Equivalencia propuesta Switch

| Android | Switch port |
|---|---|
| Activities / Leanback | Borealis Activity/View |
| Retrofit/OkHttp-like HTTP | libcurl wrapper (fase 2) |
| IJKPlayer / ExoPlayer | libmpv + FFmpeg |
| SharedPreferences / DB | JSON/SQLite local |
| Android TV remote keys | Joy-Con / Pro Controller via Borealis |
| APK assets | RomFS resources |

## Restricciones

No se implementan tecnicas para saltar autenticacion, DRM o controles de acceso. El cliente debe utilizar el flujo normal del servicio y contenido autorizado.
