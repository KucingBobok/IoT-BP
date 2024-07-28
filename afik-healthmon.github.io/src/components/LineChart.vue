<template>
  <Line :data="chartDataComputed" :options="chartOptions" />
</template>

<script>
import { Line } from "vue-chartjs";
import {
  Chart,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
} from "chart.js";

Chart.register(
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
      default: "", // Provide a default value if not provided
    },
    chartData: {
      type: Array,
      required: true,
    },
    options: {
      type: Object,
      default: () => ({}), // Provide a default value if not provided
    },
  },
  data() {
    return {
      chartOptions: this.options,
    };
  },
  computed: {
    chartDataComputed() {
      const times = this.chartData.map((d) => d.time);
      const totals = this.chartData.map((d) => d.total);
      // console.log(totals);
      // console.log(times);
      return {
        labels: times,
        datasets: [
          {
            label: this.label,
            data: totals,
          },
        ],
      };
    },
  },
  // mounted() {
  //   this.renderChart(this.chartDataComputed, this.chartOptions);
  // },
};
</script>
