<script setup>
import { computed } from 'vue'
import { withBase, useData } from 'vitepress'
import Cards from './Cards.vue'

const { isDark } = useData()

const skillsIcons =
  'windows,linux,ubuntu,python,vscode,bash,powershell,arduino'

const skillsSrc = computed(() => {
  const baseUrl = `https://skillicons.dev/icons?i=${skillsIcons}`

  return isDark.value
    ? `${baseUrl}&theme=dark`
    : `${baseUrl}&theme=light`
})

const featureCards = [
  {
    title: 'Adquisición de datos',
    description:
      'Aprende a trabajar con sensores, ADC/DAC, acondicionamiento de señal y conversión analógica-digital.',
  },
  {
    title: 'Comunicación de sensores',
    description:
      'Interfaces I²C y SPI para lectura de sensores digitales y visualización en pantallas OLED.',
  },
  {
    title: 'Conectividad IoT',
    description:
      'Wi-Fi y Bluetooth LE para publicación de datos y comunicación con dispositivos móviles y plataformas IoT.',
  },
  {
    title: 'Placa Pulsar C6',
    description:
      'Ejemplos específicos optimizados para la placa Pulsar C6 basada en ESP32-C6 con soporte RISC-V.',
  },
]
</script>

<template>
  <main class="home">
    <section class="hero-section">
      <!-- Placa superior izquierda -->
      <img
        class="corner-board corner-board-left"
        :src="withBase('/pulsar-c6.png')"
        alt=""
        aria-hidden="true"
      />

      <!-- Placa superior derecha -->
      <img
        class="corner-board corner-board-right"
        :src="withBase('/pulsar-c6.png')"
        alt=""
        aria-hidden="true"
      />

      <div class="hero">
        <h1>ESP32-C6 Pulsar Labs</h1>

        <p class="description">
          Documentación práctica para aprender adquisición de datos,
          sensores, comunicación I²C/SPI, Wi-Fi, Bluetooth LE y plataformas
          IoT con la placa Pulsar C6.
        </p>

        <div class="actions">
          <a
            :href="withBase('/pulsar-c6-getting-started')"
            class="btn primary"
          >
            Primeros pasos con la Pulsar C6
          </a>

          <a
            :href="withBase('/examples')"
            class="btn secondary"
          >
            Ver prácticas
          </a>
        </div>

        <div class="skills">
          <p>Funciona con las siguientes herramientas</p>

          <div class="skills-link">
            <img
              :key="skillsSrc"
              :src="skillsSrc"
              alt="Windows, Linux, Ubuntu, Python, Visual Studio Code, Bash, PowerShell y Arduino"
            />
          </div>
        </div>
      </div>
    </section>

    <section class="cards-section">
      <Cards :cards="featureCards" />
    </section>
  </main>
</template>

<style scoped>
.home {
  position: relative;
  width: 100%;
  min-height: calc(100vh - 64px);
  overflow: hidden;
  background-color: var(--vp-c-bg);
}

/* =====================================================
   HERO
   ===================================================== */

.hero-section {
  position: relative;
  min-height: calc(100vh - 64px);
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 0 24px 72px;
  overflow: hidden;
  background-color: var(--vp-c-bg);
}

.hero {
  position: relative;
  z-index: 2;
  width: 100%;
  max-width: 760px;
  text-align: center;
}

/* =====================================================
   IMÁGENES DE LAS ESQUINAS
   ===================================================== */

/* =====================================================
   IMÁGENES DE LAS ESQUINAS
   ===================================================== */

.corner-board {
  --board-opacity: 1;

  position: absolute;
  top: -108px;
  z-index: 1;
  display: block;
  width: clamp(300px, 28vw, 420px);
  height: auto;
  pointer-events: none;
  user-select: none;
  opacity: 0;
  will-change: opacity, transform;
  backface-visibility: hidden;
}

.corner-board-left {
  left: -120px;

  animation:
    board-enter-left
    900ms
    cubic-bezier(0.22, 1, 0.36, 1)
    500ms
    both;
}

.corner-board-right {
  right: -120px;
  transform: scaleX(-1);

  animation:
    board-enter-right
    900ms
    cubic-bezier(0.22, 1, 0.36, 1)
    220ms
    both;
}

@keyframes board-enter-left {
  from {
    opacity: 0;
    transform:
      translate3d(-36px, -22px, 0)
      scale(0.96);
  }

  to {
    opacity: var(--board-opacity);
    transform:
      translate3d(0, 0, 0)
      scale(1);
  }
}

@keyframes board-enter-right {
  from {
    opacity: 0;
    transform:
      translate3d(36px, -22px, 0)
      scaleX(-1)
      scale(0.96);
  }

  to {
    opacity: var(--board-opacity);
    transform:
      translate3d(0, 0, 0)
      scaleX(-1)
      scale(1);
  }
}

:global(.dark) .corner-board {
  --board-opacity: 0.82;
}

h1 {
  max-width: 760px;
  margin: 0 auto;
  color: var(--vp-c-text-1);
  font-size: clamp(40px, 5.2vw, 64px);
  line-height: 1.05;
  letter-spacing: -0.055em;
  font-weight: 650;
}

.description {
  max-width: 640px;
  margin: 24px auto 0;
  color: var(--vp-c-text-2);
  font-size: 19px;
  line-height: 1.65;
  font-weight: 450;
}

.actions {
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 14px;
  margin-top: 34px;
  flex-wrap: wrap;
}

.btn {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  min-height: 42px;
  padding: 0 22px;
  border: 1px solid transparent;
  border-radius: 999px;
  font-size: 14px;
  font-weight: 700;
  line-height: 1;
  text-decoration: none;
  transition:
    transform 0.18s ease,
    background-color 0.18s ease,
    border-color 0.18s ease,
    color 0.18s ease;
}

.btn:hover {
  transform: translateY(-2px);
}

.btn:focus-visible {
  outline: 3px solid rgba(153, 112, 0, 0.25);
  outline-offset: 3px;
}

.primary {
  color: #ffffff;
  background-color: #997000;
  border-color: #997000;
}

.primary:hover {
  color: #ffffff;
  background-color: #836000;
  border-color: #836000;
}

.secondary {
  color: var(--vp-c-text-1);
  background-color: transparent;
  border-color: var(--vp-c-divider);
}

.secondary:hover {
  color: var(--vp-c-text-1);
  border-color: #997000;
}

.skills {
  margin-top: 72px;
}

.skills p {
  margin: 0 0 18px;
  color: var(--vp-c-text-3);
  font-size: 13px;
  line-height: 1.4;
  font-weight: 450;
}

.skills-link {
  display: inline-flex;
  align-items: center;
  justify-content: center;
}

.skills-link img {
  display: block;
  width: auto;
  max-width: 100%;
  height: 48px;
}

.cards-section {
  position: relative;
  z-index: 3;
  width: 100%;
  margin-top: -170px;
  padding: 0 24px 54px;
  background-color: transparent;
}

@media (max-width: 1000px) {
  .hero-section {
    padding-top: 112px;
  }

    .corner-board {
    --board-opacity: 0.9;

    top: -38px;
    width: 190px;
    }

  .corner-board-left {
    left: -105px;
  }

  .corner-board-right {
    right: -105px;
  }

  h1 {
    font-size: clamp(40px, 7vw, 56px);
  }
}

@media (max-width: 640px) {
  .hero-section {
    min-height: calc(100vh - 64px);
    padding: 142px 20px 72px;
  }

  .corner-board {
    top: -38px;
    width: 190px;
    opacity: 0.9;
  }

  .corner-board-left {
    left: -78px;
  }

  .corner-board-right {
    right: -78px;
  }

  :global(.dark) .corner-board {
    opacity: 0.82;
  }

  h1 {
    font-size: clamp(36px, 11vw, 48px);
    line-height: 1.08;
    letter-spacing: -0.045em;
  }

  .description {
    max-width: 520px;
    margin-top: 22px;
    font-size: 17px;
    line-height: 1.6;
  }

  .actions {
    gap: 12px;
    margin-top: 30px;
  }

  .btn {
    min-height: 44px;
    padding: 0 19px;
    font-size: 13px;
  }

  .skills {
    margin-top: 56px;
  }

  .skills-link img {
    height: 42px;
  }

  .cards-section {
    padding: 0 20px 72px;
  }
}

@media (max-width: 420px) {
  .hero-section {
    padding-top: 132px;
  }

  .corner-board {
    top: -25px;
    width: 155px;
  }

  .corner-board-left {
    left: -66px;
  }

  .corner-board-right {
    right: -66px;
  }

  .actions {
    flex-direction: column;
  }

  .btn {
    width: 100%;
    max-width: 290px;
  }

  .skills-link img {
    height: auto;
  }
}

@media (prefers-reduced-motion: reduce) {
  .corner-board {
    animation: none;
    opacity: var(--board-opacity);
  }

  .corner-board-left {
    transform: none;
  }

  .corner-board-right {
    transform: scaleX(-1);
  }

  .btn {
    transition: none;
  }

  .btn:hover {
    transform: none;
  }
}
</style>