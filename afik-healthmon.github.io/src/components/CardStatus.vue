<style scoped>
.row {
  width: 30rem;
  align-items: center;
}
</style>

<template>
  <div class="text-center">
    <div class="container">
      <div class="row justify-content-center align-items-center">
        <h4>Real-Time Measurement</h4>
        <div class="card col-sm-4">
          <div class="card-body">
            <h5 class="card-title">Systolic</h5>
            <h4 class="card-text">{{ SYS }}</h4>
            <p class="card-text mb-2 text-muted">{{ TIME }}</p>
          </div>
        </div>
        <div class="card col-sm-4">
          <div class="card-body">
            <h5 class="card-title">MAP</h5>
            <h4 class="card-text">{{ MAP }}</h4>
            <p class="card-text mb-2 text-muted">{{ TIME }}</p>
          </div>
        </div>
        <div class="card col-sm-4">
          <div class="card-body">
            <h5 class="card-title">Diastolic</h5>
            <h4 class="card-text">{{ DIA }}</h4>
            <p class="card-text mb-2 text-muted">{{ TIME }}</p>
          </div>
        </div>
        <div class="card col-sm-4">
          <div class="card-body">
            <h5 class="card-title">Heart Rate</h5>
            <h4 class="card-text">{{ HR }}</h4>
            <p class="card-text mb-2 text-muted">{{ TIME }}</p>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  props: {
    rawData: {
      type: Object,
      required: true,
    },
  },
  data() {
    return {
      Data: {},
      MAP: [],
      SYS: [],
      DIA: [],
      HR: [],
      TIME: [],
    };
  },
  watch: {
    rawData: {
      handler(newData) {
        // Update your component with the new data
        this.updateComponent(newData);
      },
      deep: true, // Watch for nested changes
    },
  },
  methods: {
    updateComponent(newData) {
      const newestData = Object.values(newData).reduce((a, b) => {
        return parseInt(b.timestamp) > parseInt(a.timestamp) ? b : a;
      });
      const date = new Date(parseInt(newestData.timestamp) * 1000);
      const formattedTime = `${(date.getMonth() + 1)
        .toString()
        .padStart(2, "0")}/${date.getDate().toString().padStart(2, "0")} ${date
        .getHours()
        .toString()
        .padStart(2, "0")}:${date
        .getMinutes()
        .toString()
        .padStart(2, "0")}:${date.getSeconds().toString().padStart(2, "0")}`;
      this.Data = `MAP: ${newestData.map}, Systolic: ${newestData.sistole}, Diastolic: ${newestData.diastole}, HR: ${newestData.hr}, Time: ${formattedTime}`;
      this.SYS = newestData.sistole;
      this.DIA = newestData.diastole;
      this.HR = newestData.hr;
      this.MAP = newestData.map;

      const [dateFormat, timeString] = formattedTime.split(" ");
      const [month, day] = dateFormat.split("/");
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
      const monthName = monthNames[month - 1];
      const [hour, minute, second] = timeString.split(":");
      this.TIME = `${day} ${monthName} ${hour}:${minute}:${second}`;
    },
  },
};
</script>
