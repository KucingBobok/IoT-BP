<template>
  <Line :data="chart" :options="chartOptions" />
</template>

<script>
import { Line } from "vue-chartjs";
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
} from "chart.js";

ChartJS.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend
);

export default {
  components: { Line },
  props: {
    label: {
      type: String,
    },
    chartData: {
      type: Array,
      required: true,
    },
    options: {
      type: Object,
    },
  },
  data() {
    return {
      chart: {
        labels: this.dates,
        datasets: {
          label: this.label,
          data: this.totals,
        },
      },
      chartOptions: this.options,
    };
  },
  computed() {
    const dates = this.chartData.map((d) => d.date).reverse();
    const totals = this.chartData.map((d) => d.total).reverse();
    console.log(this.chartData);
    console.log(dates);
    console.log(totals);
    this.renderChart(
      {
        labels: dates,
        datasets: [
          {
            label: this.label,
            data: totals,
          },
        ],
      },
      this.options
    );
  },
};
</script>
