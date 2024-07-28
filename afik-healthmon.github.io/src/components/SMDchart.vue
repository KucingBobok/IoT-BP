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
      type: Object,
    },
    options: {
      type: Object,
      default: () => ({}), // Provide a default value if not provided
    },
  },
  data() {
    return {
      chartOptions: this.options,
      //   chartDataComputed: this.chartData,
    };
  },
  computed: {
    chartDataComputed() {
      console.log(this.chartData);
      return this.chartData;
    },
  },
};
</script>
