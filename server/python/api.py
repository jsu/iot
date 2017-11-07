from flask import Flask, request
from flask_restful import Resource, Api
import MySQLdb
import paho.mqtt.publish as publish

db_user = 'iot'
db_pass = 'Kw/hy7D8JB*^4?IX+daDC+NJV$n5C$ofSBZQ4rOB|hyUgzDBMyIx4&O1YE0e2j7x'
db_name = 'iot'
db=MySQLdb.connect(user=db_user, passwd=db_pass, db=db_name)
db.autocommit(True)

app = Flask(__name__)
api = Api(app)

MQTT = "127.0.0.1"
PREFIX = "XYCS"

class CommandMachine(Resource):
    def post(self, sn):
        publish.single("{}/{}/command/{}".format(PREFIX, sn, "machine"),
                request.form["data"], hostname=MQTT)
        return {"data": request.form["data"]}

class CommandOpMode(Resource):
    def post(self, sn):
        publish.single("{}/{}/command/{}".format(PREFIX, sn, "opmode"),
                request.form["data"], hostname=MQTT)
        return {"data": request.form["data"]}

class CommandFan(Resource):
    def post(self, sn):
        publish.single("{}/{}/command/{}".format(PREFIX, sn, "fan"),
                request.form["data"], hostname=MQTT)
        return {"data": request.form["data"]}

class CommandEP(Resource):
    def post(self, sn):
        publish.single("{}/{}/command/{}".format(PREFIX, sn, "EP"),
                request.form["data"], hostname=MQTT)
        return {"data": request.form["data"]}

class CommandUV(Resource):
    def post(self, sn):
        publish.single("{}/{}/command/{}".format(PREFIX, sn, "UV"),
                request.form["data"], hostname=MQTT)
        return {"data": request.form["data"]}

class CommandOzone(Resource):
    def post(self, sn):
        publish.single("{}/{}/command/{}".format(PREFIX, sn, "ozone"),
                request.form["data"], hostname=MQTT)
        return {"data": request.form["data"]}

class CommandIon(Resource):
    def post(self, sn):
        publish.single("{}/{}/command/{}".format(PREFIX, sn, "ion"),
                request.form["data"], hostname=MQTT)
        return {"data": request.form["data"]}

class CommandBuzzer(Resource):
    def post(self, sn):
        publish.single("{}/{}/command/{}".format(PREFIX, sn, "buzzer"),
                request.form["data"], hostname=MQTT)
        return {"data": request.form["data"]}

class CommandPwmDuty(Resource):
    def post(self, sn):
        publish.single("{}/{}/command/{}".format(PREFIX, sn, "pwm_duty"),
                request.form["data"], hostname=MQTT)
        return {"data": request.form["data"]}

api.add_resource(CommandMachine, '/XYCS/<string:sn>/command/machine')
api.add_resource(CommandOpMode, '/XYCS/<string:sn>/command/opmode')
api.add_resource(CommandFan, '/XYCS/<string:sn>/command/fan')
api.add_resource(CommandEP, '/XYCS/<string:sn>/command/EP')
api.add_resource(CommandUV, '/XYCS/<string:sn>/command/UV')
api.add_resource(CommandOzone, '/XYCS/<string:sn>/command/ozone')
api.add_resource(CommandIon, '/XYCS/<string:sn>/command/ion')
api.add_resource(CommandBuzzer, '/XYCS/<string:sn>/command/buzzer')
api.add_resource(CommandPwmDuty, '/XYCS/<string:sn>/command/pwm_duty')

class StatusMachine(Resource):
    def get(self, sn):
        c = db.cursor()
        c.execute("""select value from status_value where client_id = %s and status_id = (select id from status where name = %s) order by created_at desc limit 1;""", (sn, "machine"))
        return {"data": c.fetchone()[0]}

class StatusOpMode(Resource):
    def get(self, sn):
        c = db.cursor()
        c.execute("""select value from status_value where client_id = %s and status_id = (select id from status where name = %s) order by created_at desc limit 1;""", (sn, "opmode"))
        return {"data": c.fetchone()[0]}

class StatusFan(Resource):
    def get(self, sn):
        c = db.cursor()
        c.execute("""select value from status_value where client_id = %s and status_id = (select id from status where name = %s) order by created_at desc limit 1;""", (sn, "fan"))
        return {"data": c.fetchone()[0]}

class StatusEP(Resource):
    def get(self, sn):
        c = db.cursor()
        c.execute("""select value from status_value where client_id = %s and status_id = (select id from status where name = %s) order by created_at desc limit 1;""", (sn, "EP"))
        return {"data": c.fetchone()[0]}

class StatusUV(Resource):
    def get(self, sn):
        c = db.cursor()
        c.execute("""select value from status_value where client_id = %s and status_id = (select id from status where name = %s) order by created_at desc limit 1;""", (sn, "UV"))
        return {"data": c.fetchone()[0]}

class StatusOzone(Resource):
    def get(self, sn):
        c = db.cursor()
        c.execute("""select value from status_value where client_id = %s and status_id = (select id from status where name = %s) order by created_at desc limit 1;""", (sn, "ozone"))
        return {"data": c.fetchone()[0]}

class StatusIon(Resource):
    def get(self, sn):
        c = db.cursor()
        c.execute("""select value from status_value where client_id = %s and status_id = (select id from status where name = %s) order by created_at desc limit 1;""", (sn, "ion"))
        return {"data": c.fetchone()[0]}

class StatusBuzzer(Resource):
    def get(self, sn):
        c = db.cursor()
        c.execute("""select value from status_value where client_id = %s and status_id = (select id from status where name = %s) order by created_at desc limit 1;""", (sn, "buzzer"))
        return {"data": c.fetchone()[0]}

class StatusPwmDuty(Resource):
    def get(self, sn):
        c = db.cursor()
        c.execute("""select value from status_value where client_id = %s and status_id = (select id from status where name = %s) order by created_at desc limit 1;""", (sn, "pwm_duty"))
        return {"data": c.fetchone()[0]}

api.add_resource(StatusMachine, '/XYCS/<string:sn>/status/machine')
api.add_resource(StatusOpMode, '/XYCS/<string:sn>/status/opmode')
api.add_resource(StatusFan, '/XYCS/<string:sn>/status/fan')
api.add_resource(StatusEP, '/XYCS/<string:sn>/status/EP')
api.add_resource(StatusUV, '/XYCS/<string:sn>/status/UV')
api.add_resource(StatusOzone, '/XYCS/<string:sn>/status/ozone')
api.add_resource(StatusIon, '/XYCS/<string:sn>/status/ion')
api.add_resource(StatusBuzzer, '/XYCS/<string:sn>/status/buzzer')
api.add_resource(StatusPwmDuty, '/XYCS/<string:sn>/status/pwm_duty')

class SensorTemperature(Resource):
    def get(self, sn):
        c = db.cursor()
        c.execute("""select value from sensor_value where client_id = %s and sensor_id = (select id from sensor where name = %s) order by created_at desc limit 1;""", (sn, "temperature"))
        return {"data": c.fetchone()[0]}

class SensorHumidity(Resource):
    def get(self, sn):
        c = db.cursor()
        c.execute("""select value from sensor_value where client_id = %s and sensor_id = (select id from sensor where name = %s) order by created_at desc limit 1;""", (sn, "humidity"))
        return {"data": c.fetchone()[0]}

class SensorFanCurrent(Resource):
    def get(self, sn):
        c = db.cursor()
        c.execute("""select value from sensor_value where client_id = %s and sensor_id = (select id from sensor where name = %s) order by created_at desc limit 1;""", (sn, "fan_current"))
        return {"data": c.fetchone()[0]}

class SensorPrdCurrent(Resource):
    def get(self, sn):
        c = db.cursor()
        c.execute("""select value from sensor_value where client_id = %s and sensor_id = (select id from sensor where name = %s) order by created_at desc limit 1;""", (sn, "prd_current"))
        return {"data": c.fetchone()[0]}

class SensorUV(Resource):
    def get(self, sn):
        c = db.cursor()
        c.execute("""select value from sensor_value where client_id = %s and sensor_id = (select id from sensor where name = %s) order by created_at desc limit 1;""", (sn, "UV"))
        return {"data": c.fetchone()[0]}

class SensorOzone(Resource):
    def get(self, sn):
        c = db.cursor()
        c.execute("""select value from sensor_value where client_id = %s and sensor_id = (select id from sensor where name = %s) order by created_at desc limit 1;""", (sn, "ozone"))
        return {"data": c.fetchone()[0]}

class SensorPM25(Resource):
    def get(self, sn):
        c = db.cursor()
        c.execute("""select value from sensor_value where client_id = %s and sensor_id = (select id from sensor where name = %s) order by created_at desc limit 1;""", (sn, "PM25"))
        return {"data": c.fetchone()[0]}

class SensorMotion(Resource):
    def get(self, sn):
        c = db.cursor()
        c.execute("""select value from sensor_value where client_id = %s and sensor_id = (select id from sensor where name = %s) order by created_at desc limit 1;""", (sn, "motion"))
        return {"data": c.fetchone()[0]}

api.add_resource(SensorTemperature, '/XYCS/<string:sn>/sensor/temperature')
api.add_resource(SensorHumidity, '/XYCS/<string:sn>/sensor/humidity')
api.add_resource(SensorFanCurrent, '/XYCS/<string:sn>/sensor/fan_current')
api.add_resource(SensorPrdCurrent, '/XYCS/<string:sn>/sensor/prd_current')
api.add_resource(SensorUV, '/XYCS/<string:sn>/sensor/UV')
api.add_resource(SensorOzone, '/XYCS/<string:sn>/sensor/ozone')
api.add_resource(SensorPM25, '/XYCS/<string:sn>/sensor/PM25')
api.add_resource(SensorMotion, '/XYCS/<string:sn>/sensor/motion')

if __name__ == '__main__':
    app.run(debug=True);
