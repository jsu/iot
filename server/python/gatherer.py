import MySQLdb
import paho.mqtt.client as mqtt

db_user = 'iot'
db_pass = 'Kw/hy7D8JB*^4?IX+daDC+NJV$n5C$ofSBZQ4rOB|hyUgzDBMyIx4&O1YE0e2j7x'
db_name = 'iot'
db=MySQLdb.connect(user=db_user, passwd=db_pass, db=db_name)
db.autocommit(True)

def on_message(client, userdata, msg):
    '''
    print("message topic=", msg.topic)
    print("message received=" , str(msg.payload.decode('UTF-8')))
    '''
    _, client_id, msg_type, msg_name = msg.topic.split('/')
    c = db.cursor()
    if msg_type == "sensor":
        c.execute(
            """update sensor_value set value = %s where client_id = %s and sensor_id = (select id from sensor where name = %s);""",
            (str(msg.payload.decode('UTF-8')), client_id, msg_name))
    elif msg_type == "status":
        c.execute(
            """update status_value set value = %s where client_id = %s and status_id = (select id from status where name = %s);""",
            (str(msg.payload.decode('UTF-8')), client_id, msg_name))

if __name__ == '__main__':
    client_name = "Gatherer"
    mqtt_host = "127.0.0.1"
    mqtt_port = 1883
    client = mqtt.Client(client_name)
    client.on_message = on_message
    client.connect(mqtt_host, mqtt_port)
    client.subscribe("XYCS/#")
    client.loop_forever()
