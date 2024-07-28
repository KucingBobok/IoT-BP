<template>
  <header class="col text-center">
    <img
      alt="Health Logo"
      class="logo"
      src="./assets/images.svg"
      width="125"
      height="125"
    />
    <h2>Health Monitor</h2>

    <!-- <div class="wrapper"> -->
    <!-- <HelloWorld msg="You did it!" /> -->
    <!-- </div> -->
  </header>
  <body></body>
  <main>
    <div id="app" class="container">
      <div class="row mt-5" v-if="arrMap.length > 0">
        <div class="col">
          <h2>Map</h2>
          <line-chart
            :chartData="arrMap"
            :options="chartOptions"
            label="MAP"
          ></line-chart>
        </div>
      </div>

      <div class="row mt-5" v-if="arrSystolic.length > 0">
        <div class="col">
          <h2>Systolic</h2>
          <line-chart
            :chartData="arrSystolic"
            :options="chartOptions"
            label="SYSTOLIC"
          ></line-chart>
        </div>
      </div>

      <div class="row mt-5" v-if="arrDiastolic.length > 0">
        <div class="col">
          <h2>Diastolic</h2>
          <line-chart
            :chartData="arrDiastolic"
            :options="chartOptions"
            label="DIASTOLIC"
          ></line-chart>
        </div>
      </div>

      <div class="row mt-5" v-if="arrHr.length > 0">
        <div class="col">
          <h2>Heart Rate</h2>
          <line-chart
            :chartData="arrHr"
            :options="chartOptions"
            label="HEARTRATE"
          ></line-chart>
        </div>
      </div>
    </div>
  </main>
</template>

<script>
import HelloWorld from "./components/HelloWorld.vue";
import TheWelcome from "./components/TheWelcome.vue";

import db from "./utl/firebase.js";

import { onValue, get, ref } from "firebase/database";
import LineChart from "./components/LineChart.vue";

export default {
  name: "app",
  components: {
    LineChart,
  },
  data() {
    return {
      arrMap: [],
      arrSystolic: [],
      arrDiastolic: [],
      arrHr: [],
      chartOptions: {
        responsive: true,
        maintainAspectRatio: true,
      },
    };
  },
  async created() {
    const testData = ref(db, "afik");

    onValue(testData, (snapshot) => {
      if (snapshot.exists()) {
        const data = snapshot.val();
        // console.log(data);
        this.processData(data);
      } else {
        console.log("No data available");
      }
    });
  },
  methods: {
    processData(data) {
      this.arrMap.splice(0, this.arrMap.length);
      this.arrSystolic.splice(0, this.arrSystolic.length);
      this.arrDiastolic.splice(0, this.arrDiastolic.length);
      this.arrHr.splice(0, this.arrHr.length);

      Object.values(data).forEach((d) => {
        const unix = new Date(d.timestamp * 1000);
        const date = `${(unix.getMonth() + 1)
          .toString()
          .padStart(2, "0")}/${unix
          .getDate()
          .toString()
          .padStart(2, "0")} - ${unix
          .getHours()
          .toString()
          .padStart(2, "0")}:${unix.getMinutes().toString().padStart(2, "0")}`;
        const { map, sistole, diastole, hr } = d;

        this.arrMap.push({ date, total: map });
        this.arrSystolic.push({ date, total: sistole });
        this.arrDiastolic.push({ date, total: diastole });
        this.arrHr.push({ date, total: hr });
      });
      // Vue.set(this, "arrHr", this.arrHr);
      // console.log(this.arrMap); // Log to see the pushed data
      console.log(this.arrDiastolic); // Log to see the pushed data
      console.log(this.arrHr);
    },
  },
};
</script>

<style></style>
