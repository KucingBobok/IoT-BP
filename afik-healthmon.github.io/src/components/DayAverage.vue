<template>
  <h2>{{ formattedDateMeasure }}</h2>
  <p>
    Blood Pressure: {{ averageSystolic }} / {{ averageMAP }} /
    {{ averageDiastolic }}
  </p>
  <p>Heartrate: {{ averageHeartrate }}</p>
</template>

<script>
export default {
  props: {
    Systolic: {
      type: Array,
    },
    Diastolic: {
      type: Array,
    },
    MAP: {
      type: Array,
    },
    Heartrate: {
      type: Array,
    },
    dateMeasure: {
      type: String,
    },
  },
  computed: {
    formattedDateMeasure() {
      if (this.dateMeasure && this.dateMeasure.trim() !== "") {
        const date = new Date(this.dateMeasure);
        const monthNames = [
          "January",
          "February",
          "March",
          "April",
          "May",
          "June",
          "July",
          "August",
          "September",
          "October",
          "November",
          "December",
        ];
        return `${date.getDate()} ${monthNames[date.getMonth()]} Average`;
      } else {
        return `Daily Average`;
      }
    },
    averageSystolic() {
      return (
        this.$props.Systolic.reduce((a, b) => a + parseInt(b.total), 0) /
        this.$props.Systolic.length
      ).toFixed(0);
    },
    averageDiastolic() {
      return (
        this.$props.Diastolic.reduce((a, b) => a + parseInt(b.total), 0) /
        this.$props.Diastolic.length
      ).toFixed(0);
    },
    averageMAP() {
      return (
        this.$props.MAP.reduce((a, b) => a + parseInt(b.total), 0) /
        this.$props.MAP.length
      ).toFixed(0);
    },
    averageHeartrate() {
      return (
        this.$props.Heartrate.reduce((a, b) => a + parseInt(b.total), 0) /
        this.$props.Heartrate.length
      ).toFixed(0);
    },
  },
};
</script>
