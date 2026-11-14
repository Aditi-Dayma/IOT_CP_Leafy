---
name: Bio-Telemetry Precision
colors:
  surface: '#faf8ff'
  surface-dim: '#d2d9f4'
  surface-bright: '#faf8ff'
  surface-container-lowest: '#ffffff'
  surface-container-low: '#f2f3ff'
  surface-container: '#eaedff'
  surface-container-high: '#e2e7ff'
  surface-container-highest: '#dae2fd'
  on-surface: '#131b2e'
  on-surface-variant: '#3c4a42'
  inverse-surface: '#283044'
  inverse-on-surface: '#eef0ff'
  outline: '#6c7a71'
  outline-variant: '#bbcabf'
  surface-tint: '#006c49'
  primary: '#006c49'
  on-primary: '#ffffff'
  primary-container: '#10b981'
  on-primary-container: '#00422b'
  inverse-primary: '#4edea3'
  secondary: '#006c4a'
  on-secondary: '#ffffff'
  secondary-container: '#82f5c1'
  on-secondary-container: '#00714e'
  tertiary: '#855300'
  on-tertiary: '#ffffff'
  tertiary-container: '#e29100'
  on-tertiary-container: '#523200'
  error: '#ba1a1a'
  on-error: '#ffffff'
  error-container: '#ffdad6'
  on-error-container: '#93000a'
  primary-fixed: '#6ffbbe'
  primary-fixed-dim: '#4edea3'
  on-primary-fixed: '#002113'
  on-primary-fixed-variant: '#005236'
  secondary-fixed: '#85f8c4'
  secondary-fixed-dim: '#68dba9'
  on-secondary-fixed: '#002114'
  on-secondary-fixed-variant: '#005137'
  tertiary-fixed: '#ffddb8'
  tertiary-fixed-dim: '#ffb95f'
  on-tertiary-fixed: '#2a1700'
  on-tertiary-fixed-variant: '#653e00'
  background: '#faf8ff'
  on-background: '#131b2e'
  surface-variant: '#dae2fd'
typography:
  display-lg:
    fontFamily: Plus Jakarta Sans
    fontSize: 3rem
    fontWeight: '700'
    lineHeight: 3.5rem
    letterSpacing: -0.025em
  headline-lg:
    fontFamily: Plus Jakarta Sans
    fontSize: 2.25rem
    fontWeight: '600'
    lineHeight: 2.75rem
    letterSpacing: -0.02em
  headline-lg-mobile:
    fontFamily: Plus Jakarta Sans
    fontSize: 1.75rem
    fontWeight: '600'
    lineHeight: 2.25rem
    letterSpacing: -0.015em
  headline-md:
    fontFamily: Plus Jakarta Sans
    fontSize: 1.5rem
    fontWeight: '600'
    lineHeight: 2rem
    letterSpacing: -0.015em
  headline-sm:
    fontFamily: Plus Jakarta Sans
    fontSize: 1.125rem
    fontWeight: '600'
    lineHeight: 1.5rem
    letterSpacing: -0.01em
  body-lg:
    fontFamily: Inter
    fontSize: 1rem
    fontWeight: '400'
    lineHeight: 1.5rem
  body-md:
    fontFamily: Inter
    fontSize: 0.875rem
    fontWeight: '400'
    lineHeight: 1.25rem
  body-sm:
    fontFamily: Inter
    fontSize: 0.75rem
    fontWeight: '400'
    lineHeight: 1rem
  label-lg:
    fontFamily: Inter
    fontSize: 0.875rem
    fontWeight: '500'
    lineHeight: 1.25rem
    letterSpacing: 0.01em
  label-md:
    fontFamily: Inter
    fontSize: 0.75rem
    fontWeight: '600'
    lineHeight: 1rem
    letterSpacing: 0.02em
  label-sm:
    fontFamily: Inter
    fontSize: 0.6875rem
    fontWeight: '600'
    lineHeight: 0.875rem
    letterSpacing: 0.04em
rounded:
  sm: 0.25rem
  DEFAULT: 0.5rem
  md: 0.75rem
  lg: 1rem
  xl: 1.5rem
  full: 9999px
spacing:
  gutter: 1.5rem
  gutter-mobile: 1rem
  margin: 2rem
  margin-mobile: 1rem
  space-xs: 0.25rem
  space-sm: 0.5rem
  space-md: 1rem
  space-lg: 1.5rem
  space-xl: 2.5rem
---

## Brand & Style

This design system embodies the intersection of botanical vitality and technical precision for an IoT and MLOps automated plant cultivation platform. The aesthetic is clean, technical, and serene—marrying high-density agronomy telemetry with intuitive machine-learning operation flows.

- **Design Movement:** Clean Modern SaaS with subtle tactile precision and low-contrast borders. It avoids heavy skeletal shadows in favor of crisp white cards, structural division, and purposeful botanical status accents.
- **Audience:** Data scientists, commercial agronomists, precision farmers, and IoT system operators managing greenhouse microclimates and predictive yield pipelines.
- **Emotional Response:** Confidence, physiological vitality, absolute control, and analytical clarity. The UI feels like an enterprise-grade biological observatory.

## Colors

The palette leverages an atmospheric slate-tinted canvas paired with biological emerald greens and surgical alert tones.

- **Primary (`#10B981`) & Secondary (`#059669`):** Represents optimal vegetative health, active telemetry streams, verified model inference states, and primary executive actions. Secondary emerald is reserved for interactive hover states, dense visual charts, and focused indicators.
- **Tertiary (`#F59E0B`):** Environmental warning thresholds (e.g., vapor pressure deficit excursions, soil moisture drift, drift detection in edge inference models).
- **Critical Red (`#EF4444`):** Subsystem failures, sensor disconnects, root zone hypoxia, or immediate actuator overrides.
- **Canvas & Surfaces:** Canvas sits on an ultra-soft cool tint (`#F8FAFC`). Elevated interactive modules, sensor telemetry widgets, and data tables exist on pure white (`#FFFFFF`) to ensure maximum typographic readability and visual segmentation.
- **Borders & Dividers:** Subtle structural lines (`#E2E8F0`) separate data densities without adding visual noise.
- **Typography & Slates:** High-order headers demand deep slate-black (`#0F172A`), secondary metrics use medium slate (`#475569`), and disabled/auxiliary metadata utilizes muted slate (`#94A3B8`).

## Typography

The type system blends the contemporary, geometric humanism of **Plus Jakarta Sans** for headlines with the neutral, hyper-legible utility of **Inter** for data grids, MLOps telemetry, and body text.

- Use tabular numeric figures (`font-variant-numeric: tabular-nums`) across all sensor readouts, latency values, and ML inference probabilities to prevent jitter during real-time streaming updates.
- Headline hierarchies favor semi-bold weights with negative tracking to convey structural authority without appearing bulky.
- Metadata and pill badges prioritize high-contrast, uppercase or title-case micro typography (`label-sm`, `label-md`) with generous tracking for immediate glanceability in high-density dashboard layouts.

## Layout & Spacing

A strictly regimented 8px-based layout system tailored for complex real-time telemetry panels, pipeline DAGs, and edge fleet management.

- **Grid Architecture:** 12-column fluid grid system on desktop (`> 1280px`) with `1.5rem` gutters and `2rem` screen margins. Collapses to 6 columns on tablet (`768px - 1279px`), and a 4-column stack on mobile viewports (`< 768px`) with `1rem` gutters and margins.
- **Density Control:** Dashboard components use an interior content padding of `space-lg` (`1.5rem`) for analytical cards and charts, shrinking to `space-md` (`1rem`) inside compact sensor tables and sub-cards.
- **Reflow Rules:** Global telemetry summary bars stay sticky or docked at top; complex dual-axis charts gracefully degrade to single-variable sparklines with full metrics nested in collapsible vertical drawer views on mobile devices.

## Elevation & Depth

Visual hierarchy is achieved through crisp tonal layering and microscopic ambient light refraction rather than dark, aggressive drop-shadows.

- **Level 0 (Canvas):** The foundation substrate `#F8FAFC`. Zero elevation.
- **Level 1 (Cards & Data Panels):** Solid `#FFFFFF` fill framed with a 1px uniform outline of `#E2E8F0`. Layered with an ultra-diffuse ambient shadow: `0 1px 3px rgba(15, 23, 42, 0.04), 0 1px 2px rgba(15, 23, 42, 0.02)`.
- **Level 2 (Dropdowns, Popovers, Floating Action Drawers):** Solid `#FFFFFF` with `#E2E8F0` border and an extended ambient shadow: `0 10px 15px -3px rgba(15, 23, 42, 0.06), 0 4px 6px -4px rgba(15, 23, 42, 0.03)`.
- **Level 3 (Modal Overlays & Critical Alert Dialogs):** Centered floating dialogs with a distinct perimeter boundary (`#CBD5E1`) and deep ambient grounding: `0 20px 25px -5px rgba(15, 23, 42, 0.08), 0 8px 10px -6px rgba(15, 23, 42, 0.04)`. Accompanied by a 40% opacity slate-tinted canvas backdrop blur (`backdrop-filter: blur(4px)`).

## Shapes

The design system enforces a balanced `Rounded` language (`level 2`), using purposeful radius scaling to distinguish between structural frames, actionable controls, and diagnostic badge tokens.

- **Base Cards & Panels:** Bound by `rounded-lg` (`1rem`), offering a soft, approachable frame for dense technical diagrams.
- **Interactive Controls (Inputs, Buttons):** Standardized at `rounded` (`0.5rem`) for tactile definition and clear click targets.
- **Pills & Status Indicators:** Explicitly styled as full circular pills (`border-radius: 9999px`) to immediately set live biometric values apart from standard structural blocks.

## Components

### Buttons
- **Primary:** Filled `#10B981` with pure white text, semi-bold `label-lg`, `0.5rem` radius, with a hover transition to `#059669`. Focus states trigger a 2px `#10B981` ring offset by 2px white space.
- **Secondary:** Surface `#FFFFFF`, 1px solid `#E2E8F0`, slate `#0F172A` text. Hover shifts to `#F8FAFC` background with a `#CBD5E1` border.
- **Destructive/Override:** `#EF4444` background, white text, for immediate actuator aborts, irrigation valve shutdowns, or model rollback operations.

### Badges & Status Pills
- Always configured as full pill shapes (`rounded-full`) with a compact `space-xs` vertical and `space-sm` horizontal padding.
- Employs a low-saturation tinted background with high-contrast text and a 6px solid circular status dot:
  - *Optimal/Nominal:* `#ECFDF5` background, `#047857` text, `#10B981` pulsing status dot.
  - *Warning/Drift:* `#FFFBEB` background, `#B45309` text, `#F59E0B` status dot.
  - *Critical/Fault:* `#FEF2F2` background, `#B91C1C` text, `#EF4444` status dot.

### Cards & Analytical Modules
- Pure white `#FFFFFF` surface container, 1px `#E2E8F0` border, `1rem` corner radius, padded with `1.5rem`.
- Split headers featuring a semantic icon badge, title in `headline-sm`, and right-aligned timeframe pills or edge runtime telemetry.

### Input Fields & Controls
- **Inputs:** Background `#FFFFFF`, 1px border `#E2E8F0`, `0.5rem` corner radius, internal padding `0.5rem 0.75rem`. Text in `#0F172A` with `#94A3B8` placeholder. Active focus states produce a crisp emerald ring (`#10B981`).
- **Checkboxes & Radios:** `0.5rem` roundedness for checkboxes, full circle for radios. Default state has a 1.5px `#CBD5E1` border; checked state floods with `#10B981` containing an interior white glyph.

### Data Visualization & Sparklines
- Chart grids use dashed `#E2E8F0` rules on the Y-axis, omitting vertical gridlines to reduce clutter.
- Sensor lines utilize `#10B981` with an ultra-soft vertical gradient fill terminating at `rgba(16, 185, 129, 0.0)` for continuous streaming series. Threshold violations switch the line stroke dynamically to `#F59E0B` or `#EF4444`.