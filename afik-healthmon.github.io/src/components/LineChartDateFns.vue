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
  TimeScale,
} from "chart.js";
import "chartjs-adapter-date-fns";

Chart.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
  TimeScale
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
      chartOptions: {
        ...this.options,
        scales: {
          x: {
            type: "time",
            adapters: {
              date: {
                lib: "date-fns", // Ensure date-fns is used
              },
            },
            time: {
              unit: "hour",
              displayFormats: {
                hour: "HH:mm",
              },
              tooltipFormat: "HH:mm",
            },
            ticks: {
              autoSkip: true,
              maxTicksLimit: 24,
            },
            min: "00:00",
            max: "23:59",
          },
        },
      },
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
