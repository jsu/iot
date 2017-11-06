from flask import Flask, request
from flask_restful import Resource, Api
import paho.mqtt.publish as publish


app = Flask(__name__)
api = Api(app)

MQTT = "server.iot.pwnass.com"
PREFIX = "XYCS"

class Machine(Resource):
    def post(self, sn):
        publish.single("{}/{}/command/{}".format(PREFIX, sn, "machine"),
                request.form["data"], hostname=MQTT)
        return {"data": request.form["data"]}

class OpMode(Resource):
    def post(self, sn):
        publish.single("{}/{}/command/{}".format(PREFIX, sn, "opmode"),
                request.form["data"], hostname=MQTT)
        return {"data": request.form["data"]}

class Fan(Resource):
    def post(self, sn):
        publish.single("{}/{}/command/{}".format(PREFIX, sn, "fan"),
                request.form["data"], hostname=MQTT)

class EP(Resource):
    def post(self, sn):
        publish.single("{}/{}/command/{}".format(PREFIX, sn, "EP"),
                request.form["data"], hostname=MQTT)

class UV(Resource):
    def post(self, sn):
        publish.single("{}/{}/command/{}".format(PREFIX, sn, "UV"),
                request.form["data"], hostname=MQTT)

class Ozone(Resource):
    def post(self, sn):
        publish.single("{}/{}/command/{}".format(PREFIX, sn, "ozone"),
                request.form["data"], hostname=MQTT)

class Ion(Resource):
    def post(self, sn):
        publish.single("{}/{}/command/{}".format(PREFIX, sn, "ion"),
                request.form["data"], hostname=MQTT)

class Buzzer(Resource):
    def post(self, sn):
        publish.single("{}/{}/command/{}".format(PREFIX, sn, "buzzer"),
                request.form["data"], hostname=MQTT)

class PwmDuty(Resource):
    def post(self, sn):
        publish.single("{}/{}/command/{}".format(PREFIX, sn, "pwm_duty"),
                request.form["data"], hostname=MQTT)

api.add_resource(Machine, '/XYCS/<string:sn>/command/machine')
api.add_resource(OpMode, '/XYCS/<string:sn>/command/opmode')
api.add_resource(Fan, '/XYCS/<string:sn>/command/fan')
api.add_resource(EP, '/XYCS/<string:sn>/command/EP')
api.add_resource(UV, '/XYCS/<string:sn>/command/UV')
api.add_resource(Ozone, '/XYCS/<string:sn>/command/ozone')
api.add_resource(Ion, '/XYCS/<string:sn>/command/ion')
api.add_resource(Buzzer, '/XYCS/<string:sn>/command/buzzer')
api.add_resource(PwmDuty, '/XYCS/<string:sn>/command/pwm_duty')
if __name__ == '__main__':
    app.run(debug=True);
