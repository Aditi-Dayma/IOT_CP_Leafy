# 🌿 Leafy — Smart Plant Dashboard & Automated Microclimate Control

A high-precision **Bio-Telemetry IoT & MLOps** automated plant cultivation dashboard designed for edge greenhouse monitoring with **ESP32** and predictive microclimate control powered by **XGBoost**.

![Dashboard Preview](stitch_leafy_smart_plant_dashboard/leafy_intelligent_plant_care_automated_microclimate_control_with_night_mode/screen.png)

---

## 🚀 How to Deploy on Vercel

This dashboard is pre-configured for instant zero-config deployment on [Vercel](https://vercel.com).

### Option 1: Instant CLI Deployment (Fastest, 30 Seconds)

1. Open PowerShell or Terminal in this project folder:
   ```powershell
   cd "c:\Users\Aditi\OneDrive\Desktop\CLG\SEM-5\IOT\CP\stitch_leafy_smart_plant_dashboard"
   ```

2. Run Vercel CLI via npx (no global install needed):
   ```bash
   npx vercel
   ```

3. Follow the quick terminal prompts:
   - **Set up and deploy?** Press `Y` and Enter.
   - **Which scope do you want to deploy to?** Choose your Vercel account.
   - **Link to existing project?** Type `N`.
   - **What's your project's name?** Enter `leafy-smart-plant` (or press Enter).
   - **In which directory is your code located?** Press Enter (current directory `./`).
   - **Want to modify settings?** Type `N`.

4. Your live production URL will be generated immediately (e.g. `https://leafy-smart-plant.vercel.app`)!

---

### Option 2: Deploy via GitHub & Vercel Web Dashboard

1. Initialize git and push this repository to GitHub:
   ```bash
   git init
   git add .
   git commit -m "Initial commit: Leafy Smart Plant Dashboard"
   git branch -M main
   git remote add origin https://github.com/<your-username>/leafy-smart-plant-dashboard.git
   git push -u origin main
   ```

2. Go to **[https://vercel.com/new](https://vercel.com/new)**.
3. Select your repository `leafy-smart-plant-dashboard` and click **Import**.
4. In **Project Settings**:
   - **Framework Preset**: `Other` (Static HTML)
   - **Root Directory**: `./` (leave default)
   - **Build Command**: (leave empty or default)
   - **Output Directory**: (leave empty or default)
5. Click **Deploy**!
   - Vercel deploys within seconds and generates a free SSL domain with instant global CDN caching.

---

## 💻 Local Development

To run and preview the dashboard on your local machine:

```bash
# Using npm with the included serve runner
npm run dev

# Or with python's built-in web server
python -m http.server 3000
```
Open [http://localhost:3000](http://localhost:3000) in your browser.

---

## 🌟 Key Features

| Feature | Description |
|---|---|
| **🌙 Nighttime Greenhouse Mode** | One-click toggle between Daylight SaaS mode and low-lux bioluminescent night vision. Theme state is saved in `localStorage`. |
| **💧 Automated 5V Relay Actuator** | Interactive manual watering actuation button with live countdown timer, spinning impeller animation, and automatic root-zone moisture elevation to 54.2%. |
| **📈 XGBoost Predictive Timeline** | 12-hour sensor history + 6-hour forward predictive microclimate trajectory curve, with critical 30% moisture threshold alert intersection point. |
| **📡 Live ESP32 Telemetry Stream** | Real-time incoming packet ticker (simulates MQTT QoS 1 packets arriving every 30s) and a manual "Sync Telemetry" button. |
| **⚙️ Interactive ML Controls Modal** | Configure critical moisture trigger limits, forecast horizons (6h / 12h / 24h), and pump burst duration directly in the UI. |
| **🔬 Continuous MLOps Governance** | Retraining webhook simulation (`POST /api/v1/retrain`), MLflow Model Registry modal inspector, and Evidently AI drift audit. |

---

## 🛠 Project Structure

```
stitch_leafy_smart_plant_dashboard/
├── index.html            # Production dashboard (HTML5 + Tailwind CSS + Interactive JS)
├── vercel.json           # Vercel configuration (security headers & clean URLs)
├── package.json          # Project metadata & local dev scripts
├── .gitignore            # Git ignore file
├── README.md             # Deployment & technical documentation
└── stitch_leafy_smart_plant_dashboard/
    ├── bio_telemetry_precision/
    │   └── DESIGN.md     # Design system tokens and style specifications
    ├── leafy_intelligent_plant_care_automated_microclimate_control/
    │   ├── code.html
    │   └── screen.png
    └── leafy_intelligent_plant_care_automated_microclimate_control_with_night_mode/
        ├── code.html
        └── screen.png
```

---

## 🏷 Tech Stack

- **Frontend:** Semantic HTML5, Tailwind CSS, Google Fonts (Plus Jakarta Sans, Inter, JetBrains Mono), Material Symbols
- **Visualizations:** High-precision SVG responsive coordinate system with gradients and dynamic trigger indicators
- **Edge Hardware Protocol:** ESP32-S3, MQTT (TLS 8883), Capacitive Moisture Sensor v1.2, Photo-diode ADC, 5V DC Relay
- **Edge ML Framework:** XGBoost 2.0 Regressor, MLflow, Evidently AI
- **Deployment Target:** Vercel Edge Network

