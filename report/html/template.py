template="""
<!DOCTYPE html>
<html lang="en" dir="ltr">
    <head>
        <meta charset="utf-8">
        <title>Hashmap benchmarks</title>
        <script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.7.3/Chart.bundle.js"></script>
    </head>
    <body>
    {% for _, b in benchmarks.items() %}
    <div style="width:60%;">
        <canvas id="{{ b.name }}"></canvas>
    </div>
    {% endfor %}

    <script type="text/javascript">
    var colors = ["#dc143c", "#1e90ff", "#228b22","#ff8c00", "#ffd700","#778899", "#9370db", "#8b4513"]
    var plots = new Array()
    {% for _, b in benchmarks.items() %}
        var plot{{loop.index}} = {
            type : 'line',
            data : {
                labels : {{ b.x }},
                datasets : [
                {% set count = 0 %}
                {% for t in b.traces %}
                    {
                        label : '{{ t.name }}',
                        data : {{ t.values }},
                        fill : false,
                        backgroundColor: colors[{{ loop.index - 1 }}],
                        borderColor: colors[{{ loop.index - 1 }}],
                    },
                    {% set count = count + 1 %}
                {% endfor %}
                ]
            },
            options: {
                responsive: true,
                title: {
                    display: true,
                    text: '{{b.name}}'
                },
                tooltips: {
                    mode: 'index',
                    intersect: false,
                },
                legend : {
                    position : 'right'
                },
                hover: {
                    mode: 'nearest',
                    intersect: true
                },
                scales: {
                    xAxes: [{
                        display: true,
                        scaleLabel: {
                            display: true,
                            labelString: 'Number of elements'
                        }
                    }],
                    yAxes: [{
                        display: true,
                        scaleLabel: {
                            display: true,
                            labelString: 'Nanos per operation'
                        }
                    }]
                }
            }
        };
    {% endfor %}

    window.onload = function () {

        {% for _,b in benchmarks.items() %}
            var ctx{{loop.index}} = document.getElementById('{{b.name}}').getContext('2d');
            window.myLine = new Chart(ctx{{loop.index}}, plot{{loop.index}});
        {% endfor %}
    }

    </script>
    </body>
</html>
"""

