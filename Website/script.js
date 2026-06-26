// Firebase config
const firebaseConfig = {
  apiKey: "ADD_YOUR_FIREBASE_API_KEY",
  databaseURL: "ADD_YOUR_DATABASE_URL"
};

firebase.initializeApp(firebaseConfig);
const db = firebase.database();

console.log("Starting sign-in...");

// Simple weather predictor based on temperature and humidity
function predictWeather(temp, hum) {
  // Indoor / local microclimate approximation
  if (temp >= 30 && hum >= 60) return "HOT & HUMID";
  if (temp >= 30 && hum < 60) return "DRY HEAT";
  if (temp >= 20 && temp < 30 && hum >= 60) return "MILD & HUMID";
  if (temp >= 20 && temp < 30 && hum < 60) return "CLEAR & DRY";
  if (temp >= 10 && temp < 20 && hum >= 70) return "COOL & DAMP";
  if (temp >= 10 && temp < 20 && hum < 70) return "COOL & DRY";
  if (temp < 10) return "COLD";
  return "UNSTABLE";
}

firebase.auth().signInAnonymously()
  .then(() => {
    console.log("Signed in anonymously");

    // Use the exact path your ESP32 writes to (original code used "/value")
    const ref = db.ref("/value");

    ref.on("value", (snapshot) => {
      const data = snapshot.val();
      console.log("Raw data:", data);

      if (data) {
        const temp = data.temperature ?? data.temp ?? "--";
        const hum = data.humidity ?? data.hum ?? "--";

        // Update temperature and humidity displays
        document.getElementById("tempScreen").textContent = temp + "°C";
        document.getElementById("humScreen").textContent = hum + "%";

        // Predict weather if both values are numbers
        if (typeof temp === "number" && typeof hum === "number") {
          const condition = predictWeather(temp, hum);
          document.getElementById("weatherPrediction").textContent = condition;
        } else {
          document.getElementById("weatherPrediction").textContent = "--";
        }
      } else {
        console.warn("No data at this path yet.");
      }
    }, (error) => {
      console.error("Read error:", error.message);
    });
  })
  .catch(err => {
    console.error("Auth error:", err.message);
    console.log("Enable Anonymous sign-in in Firebase Authentication if not already done.");
  });