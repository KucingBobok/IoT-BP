// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
import { getDatabase } from "firebase/database";
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
  apiKey: "AIzaSyAyauqYEJCCeXXRlERU-N3xRRR715B0Ijg",
  authDomain: "backend-e21c9.firebaseapp.com",
  databaseURL: "https://backend-e21c9-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "backend-e21c9",
  storageBucket: "backend-e21c9.appspot.com",
  messagingSenderId: "98209739086",
  appId: "1:98209739086:web:f9dbe6128da232ea42ee1f",
  measurementId: "G-4Z9VG14G46"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const db = getDatabase(app);

export default db;