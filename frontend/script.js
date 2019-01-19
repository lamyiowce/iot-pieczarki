
let baseUrl = "http://127.0.0.1:5000";

if (!Array.prototype.last){
    Array.prototype.last = function(){
        return this[this.length - 1];
    };
}

function getDefaultGaugeSettings() {
    let myDefault = liquidFillGaugeDefaultSettings();
    myDefault.textColor = "#212121";
    myDefault.waveAnimateTime = 2500;
    myDefault.circleFillGap = 0;
    return myDefault;
}

let airHumidConfig = getDefaultGaugeSettings();
airHumidConfig.circleColor = "#0288D1";
airHumidConfig.waveTextColor = "#0c3d70";
airHumidConfig.waveColor = "#B3E5FC";
// airHumidConfig.textVertPosition = 0.2;
let airHumidGauge = loadLiquidFillGauge("air-humid-gauge", 0, airHumidConfig);

let soilHumidConfig = getDefaultGaugeSettings();
soilHumidConfig.circleColor = "#4CAF50"; // dark primary
soilHumidConfig.waveTextColor = "#388E3C"; // primary
soilHumidConfig.waveColor = "#C8E6C9"; // light primary
let soilHumidGauge = loadLiquidFillGauge("soil-humid-gauge", 0, soilHumidConfig);

let airTempConfig = getDefaultGaugeSettings();
airTempConfig.circleColor = "#FF5722"; // dark primary
airTempConfig.waveTextColor = "#E64A19"; // primary
airTempConfig.waveColor = "#FFCCBC"; // light primary
airTempConfig.waveAnimate = false;
airTempConfig.waveHeight = 0;
airTempConfig.waveAnimateTime = 10000;
airTempConfig.waveCount = 2;
airTempConfig.displayPercent = false;
airTempConfig.minValue = -5;
airTempConfig.maxValue = 40;
let airTempGauge = loadLiquidFillGauge("air-temp-gauge", 0, airTempConfig);
// Parse the date / time
let parseDate = d3.time.format("%Y-%m-%d %H:%M:%S").parse; // sample: 23-02-2018 13:45

const margin = {top: 30, right: 20, bottom: 80, left: 50};

const width = 600 - margin.left - margin.right;
const height = 270 - margin.top - margin.bottom;
// Set the ranges
let x = d3.time.scale().range([0, width]);

let y = d3.scale.linear().range([height, 0]);
// Define the axes
let xAxis = d3.svg.axis().scale(x)
    .orient("bottom");

let yAxis = d3.svg.axis().scale(y)
    .orient("left");

// Define the line
var valueline = d3.svg.line()
    .x(function(d) { return x(d.date); })
    .y(function(d) { return y(d.value); });

loadCultivations();
createGraph("air-humid-graph");
createGraph("air-temp-graph");
createGraph("soil-humid-graph");
checkForNewData();

function createGraph(graphId) {
    var svg = d3.select("#" + graphId)
        .append("svg")
        .attr("width", width + margin.left + margin.right)
        .attr("height", height + margin.top + margin.bottom)
        .append("g")
        .attr("transform",
            "translate(" + margin.left + "," + margin.top + ")");

    x.domain([0, 24*60*60*1000]);
    y.domain([0, 60]);

// Add the valueline path.
    svg.append("path")
        .attr("class", "line")
        .attr("id", graphId+"-path");
    // .attr("d", valueline(data));

// Add the X Axis
    svg.append("g")
        .attr("class", "x axis")
        .attr("transform", "translate(0," + height + ")")
        .call(xAxis)
    .selectAll("text")
        .attr("y", 0)
        .attr("x", 9)
        .attr("dy", ".35em")
        .attr("transform", "rotate(90)")
        .style("text-anchor", "start");

// Add the Y Axis
    svg.append("g")
        .attr("class", "y axis")
        .call(yAxis);
}

// Do a REST query for list of the cultivations. Expects a lists of objects with attributes id and name.
// More in sampleResponses.json
function loadCultivations(){
    const cultivationsUrl = baseUrl + "/cultivations";
    $.getJSON(cultivationsUrl)
        .done(function (cultsList) {
            const cultOpts = cultsList.map(cultElem =>
                '<option value="' + cultElem.id + '">' + cultElem.name + '</option>'
            ).join('\n');
            $("#cultivation-picker").html(cultOpts);
            getStats();
        });
}

function getStats() {
    let statsUrl = baseUrl + "/stats/" + getCultivationId();
    $.getJSON(statsUrl)
        .done(function(statsJson) {
            console.log(statsJson);
            statsJson.temp.forEach( d => d.date = parseDate(d.date));
            statsJson.airHumid.forEach( d => d.date = parseDate(d.date));
            statsJson.soilHumid.forEach( d => d.date = parseDate(d.date));
            statsJson.growth.forEach( d => d.date = parseDate(d.date));
            setTemp(statsJson.temp);
            setAirHumid(statsJson.airHumid);
            setSoilHumid(statsJson.soilHumid);
            setCurrent({
                temp: statsJson.temp.last(),
                airHumid: statsJson.airHumid.last(),
                soilHumid: statsJson.soilHumid.last(),
                growth: statsJson.growth.last()
            });
            $("time#last-check").timeago("update", new Date());
        })
}

function setCurrent({temp, airHumid, soilHumid, growth} = {}) {
    if (temp) {
        $("time#air-temp-date").timeago("update", temp.date);
        airTempGauge.update(temp.value);
    }
    if (airHumid) {
        $("time#air-humid-date").timeago("update", airHumid.date);
        airHumidGauge.update(airHumid.value);
    }
    if (soilHumid) {
        $("time#soil-humid-date").timeago("update", soilHumid.date);
        soilHumidGauge.update(soilHumid.value);
    }
    if (growth) {
        if (growth.value)
            $("#growing").text("Ready!");
        else
            $("#growing").text("Growing...");
        $("time#growing-time").timeago("update", growth.date);

    }
}

function setData(data, graphId, yExtent) {
    if (!data)
        return;
    console.log(data);
    x.domain(d3.extent(data, function(d) { return d.date; }));
    y.domain(yExtent);
    let svg = d3.select("#" + graphId).transition();
    // Make the changes
    svg.select(".line")   // change the line
        .duration(750)
        .attr("d", valueline(data));
    svg.select(".x.axis") // change the x axis
        .duration(750)
        .call(xAxis)
    .selectAll("text")
        .attr("y", 0)
        .attr("x", 9)
        .attr("dy", ".35em")
        .attr("transform", "rotate(90)")
        .style("text-anchor", "start");
    svg.select(".y.axis") // change the y axis
        .duration(750)
        .call(yAxis);
}

function setTemp(data) {
    setData(data, "air-temp-graph", [-10, 40]);
}

function setAirHumid(data) {
    setData(data, "air-humid-graph", [0, 100]);
}

function setSoilHumid(data) {
    setData(data, "soil-humid-graph", [0, 100]);
}

function getCultivationId() {
    return $("#cultivation-picker").val();
}

function NewValue() {
    if (Math.random() > .5) {
        return Math.round(Math.random() * 100);
    } else {
        return (Math.random() * 100).toFixed(1);
    }
}

function sendFreq(picker, type) {
    let url = baseUrl + "/freq/" + type + "/" + picker.value;
    let msg = $("#" + type + "-msg");
    msg.text("Wait...");
    $.get(url)
        .done(function(){ msg.text("OK!")})
        .fail(function(){ msg.text("Error.")})
        .always(function(){ sleep(10000).then(() =>
            msg.text("")
        )})
}

function sleep(milliseconds) {
  return new Promise(resolve => setTimeout(resolve, milliseconds));
}

function checkForNewData() {
    getStats();
    setTimeout(checkForNewData, 5*1000);
}