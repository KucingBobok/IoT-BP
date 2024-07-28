<style>
.container-fluid {
  margin-top: 10;
  padding-top: 100;
  background-color: aliceblue;
}
.row {
  margin-top: 100;
  padding-top: 100;
}
</style>
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
  </header>
  <aside>
    <div class="row">
      <div class="col-sm-6 d-flex justify-content-center">
        <div class="row">
          <div class="text-center">
            <h4>Sistem Monitor Kesehatan</h4>
            <div>
              <p>HOW TO:</p>
              <p>
                1. Gunakan alat pengukur pada pergelangan tangan dan ukur dengan
                meletakkan alat sejajar dengan jantung.
              </p>
              <p>
                2. Jangan bergerak hingga alat selesai mengukur, yaitu ketika
                alat tidak lagi menekan pergelangan tangan.
              </p>
              <p>
                3. Alat secara otomatis akan merekam hasil dan Anda dapat
                menampilkan hasil dengan memilih tanggal pengukuran ataupun
                melihat secara langsung.
              </p>
            </div>
          </div>
        </div>
      </div>
      <card-status :raw-data="RawData" class="col-sm-6"></card-status>
    </div>
  </aside>
  <body></body>
  <article>
    <div class="row">
      <div class="col-sm-6 text-center">
        <h6>Ubah Tanggal</h6>
        <select v-model="selectedDate" @change="updateCharts">
          <option
            v-for="(date, index) in Object.keys(dataByDate)"
            :key="index"
            :value="date"
          >
            {{ date }}
          </option>
        </select>
        <p>Pilihlah tanggal untuk melihat Average dan Chart</p>
      </div>
      <div class="col-sm-6">
        <day-average
          :Systolic="arrSystolic"
          :Diastolic="arrDiastolic"
          :MAP="arrMap"
          :Heartrate="arrHr"
          :dateMeasure="exportDate"
        ></day-average>
      </div>
    </div>
    <div id="app" class="container-fluid">
      <div class="row">
        <div class="col-sm-6" v-if="chartSMD.labels.length > 0">
          <SMDchartVue
            :chartData="chartSMD"
            :options="chartOptions"
          ></SMDchartVue>
        </div>
        <div class="col-sm-6" v-if="chartHR.labels.length > 0">
          <SMDchartVue
            :chartData="chartHR"
            :options="chartOptions"
          ></SMDchartVue>
        </div>
      </div>
    </div>
  </article>
  <footer style="padding-top: 2cap; padding-bottom: 2ch">
    <div class="text-center">
      <h6>Copyright of this website by Afik Razanafi</h6>
    </div>
  </footer>
</template>

<script>
import HelloWorld from "./components/HelloWorld.vue";
import TheWelcome from "./components/TheWelcome.vue";

import db from "./utl/firebase.js";

import { onValue, get, ref } from "firebase/database";
import LineChart from "./components/LineChart.vue";
import CardStatus from "./components/CardStatus.vue";
import DayAverage from "./components/DayAverage.vue";
import SMDchartVue from "./components/SMDchart.vue";

export default {
  name: "app",
  components: {
    LineChart,
    CardStatus,
    DayAverage,
    SMDchartVue,
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
      RawData: {},
      UploadRaw: {},
      exportDate: "",
      chartSMD: {
        labels: [],
        datasets: [],
      },
      chartHR: {
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
        this.RawData = data;
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
      console.log(this.selectedDate);
      this.exportDate = this.selectedDate;
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
      this.chartHR = {
        labels: this.arrDiastolic.map((point) => point.time),
        datasets: [
          {
            label: "Heartrate",
            data: this.arrHr.map((point) => point.total),
            borderColor: "rgba(255, 107, 107, 0.8)",
            backgroundColor: "rgba(255, 107, 107, 0.2",
          },
        ],
      };

      this.chartSMD = {
        labels: this.arrDiastolic.map((point) => point.time),
        datasets: [
          {
            label: "Systolic",
            data: this.arrSystolic.map((point) => point.total),
            borderColor: "rgba(255, 99, 132, 1)",
            backgroundColor: "rgba(255, 99, 132, 0.2)",
          },
          {
            label: "MAP",
            data: this.arrMap.map((point) => point.total),
            borderColor: "rgba(54, 162, 235, 1)",
            backgroundColor: "rgba(54, 162, 235, 0.2)",
          },
          {
            label: "Diastolic",
            data: this.arrDiastolic.map((point) => point.total),
            borderColor: "rgba(255, 206, 86, 1)",
            backgroundColor: "rgba(255, 206, 86, 0.2)",
          },
        ],
      };
      console.log(this.chartSMD);
      // Update the chart component with the new data
    },
    updateCardStatus() {
      // This method will be called when the CardStatus component updates
    },
  },
};
</script>
