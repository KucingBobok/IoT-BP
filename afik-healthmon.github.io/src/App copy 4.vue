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
    <select v-model="selectedDate" @change="updateCharts">
      <option
        v-for="(date, index) in Object.keys(dataByDate)"
        :key="index"
        :value="date"
      >
        {{ date }}
      </option>
    </select>
  </header>
  <body></body>
  <main>
    <div id="app" class="container">
      <div class="row mt-5">
        <div class="col sm-2" v-if="arrSystolic.length > 0">
          <h2>Map</h2>
          <line-chart
            :chartData="arrMap"
            :options="chartOptions"
            label="MAP"
          ></line-chart>
        </div>

        <div class="col sm-2" v-if="arrSystolic.length > 0">
          <h2>Systolic</h2>
          <line-chart
            :chartData="arrSystolic"
            :options="chartOptions"
            label="SYSTOLIC"
          ></line-chart>
        </div>

        <div class="col sm-2" v-if="arrDiastolic.length > 0">
          <h2>Diastolic</h2>
          <line-chart
            :chartData="arrDiastolic"
            :options="chartOptions"
            label="DIASTOLIC"
          ></line-chart>
        </div>

        <div class="col sm-2" v-if="arrHr.length > 0">
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
      dataByDate: {},
      selectedDate: null,
      chartData: {
        labels: [],
        datasets: [],
      },
    };
  },
  async created() {
    const testData = ref(db, "afik");

    onValue(testData, (snapshot) => {
      if (snapshot.exists()) {
        const data = snapshot.val();
        this.processData(data);
      } else {
        console.log("No data available");
      }
    });
  },
  methods: {
    processData(data) {
      const dataByDate = {};

      Object.values(data).forEach((d) => {
        const unix = new Date(d.timestamp * 1000);
        const date = `${(unix.getMonth() + 1)
          .toString()
          .padStart(2, "0")}/${unix.getDate().toString().padStart(2, "0")}`;

        if (!dataByDate[date]) {
          dataByDate[date] = {
            map: [],
            systolic: [],
            diastolic: [],
            hr: [],
          };
        }

        dataByDate[date].map.push({
          time: `${unix.getHours().toString().padStart(2, "0")}:${unix
            .getMinutes()
            .toString()
            .padStart(2, "0")}`,
          value: d.map,
        });
        dataByDate[date].systolic.push({
          time: `${unix.getHours().toString().padStart(2, "0")}:${unix
            .getMinutes()
            .toString()
            .padStart(2, "0")}`,
          value: d.sistole,
        });
        dataByDate[date].diastolic.push({
          time: `${unix.getHours().toString().padStart(2, "0")}:${unix
            .getMinutes()
            .toString()
            .padStart(2, "0")}`,
          value: d.diastole,
        });
        dataByDate[date].hr.push({
          time: `${unix.getHours().toString().padStart(2, "0")}:${unix
            .getMinutes()
            .toString()
            .padStart(2, "0")}`,
          value: d.hr,
        });
      });

      this.dataByDate = dataByDate;
      this.selectedDate = Object.keys(dataByDate)[0];
    },
    updateCharts() {
      const selectedData = this.dataByDate[this.selectedDate];

      this.arrMap = selectedData.map.map((point) => ({
        time: point.time,
        total: point.value,
      }));
      this.arrSystolic = selectedData.systolic.map((point) => ({
        time: point.time,
        total: point.value,
      }));
      this.arrDiastolic = selectedData.diastolic.map((point) => ({
        time: point.time,
        total: point.value,
      }));
      this.arrHr = selectedData.hr.map((point) => ({
        time: point.time,
        total: point.value,
      }));

      this.chartData.labels = selectedData.map.map((point) => point.time);
      this.chartData.datasets = [
        {
          label: "MAP",
          data: this.arrMap.map((point) => point.total),
        },
        {
          label: "SYSTOLIC",
          data: this.arrSystolic.map((point) => point.total),
        },
        {
          label: "DIASTOLIC",
          data: this.arrDiastolic.map((point) => point.total),
        },
        {
          label: "HEARTRATE",
          data: this.arrHr.map((point) => point.total),
        },
      ];
    },
  },
};
</script>

<style></style>
