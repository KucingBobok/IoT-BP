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
    chartCombined: {
      type: Array,
      required: true,
    },
  },
  data() {
    return {
      chartOptions: {
        responsive: true,
        maintainAspectRatio: true,
      },
    };
  },
  computed: {
    chartDataComputed() {
      const times = this.chartCombined[0].data;
      const totals = this.chartCombined.map((dataset) => dataset.data);
      console.log(chartCombined);

      return {
        labels: times,
        datasets: totals.flat(),
      };
    },
  },
};
</script>
