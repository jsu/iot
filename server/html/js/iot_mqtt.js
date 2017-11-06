// JavaScript Document

var machine_sn = "1031"; // 01: TEST MACHINE NO  product  SN "1031"

/*
function onConnect()
{
    client.subscribe("XYCS/" + machine_sn + "/#");
}

function onMessageArrived(message)
{

    var messageType = message.destinationName.substr((message.destinationName.lastIndexOf("/")-6),6) ;
    // console.log("receive : " + message.destinationName + " "+ message.payloadString);
    switch(messageType)
    {  
        case "ommand":
        case "status":
            { 
                var dest = "#" + message.destinationName.substring(
                        message.destinationName.lastIndexOf("/") + 1);
                $("#status_machine").text(message.payloadString) ;
                switch(dest)
                { 
                    case "#machine":
                        console.log("receive : " + message.destinationName + " "+ message.payloadString);
                        switch(message.payloadString)
                        {
                            case "0":
                                //$(dest).html("停機");
                                $('#machine_status').text("停機");
                                $("#status_machine").text(message.payloadString) ;

                                break;
                            case "1":
                                //$(dest).html("啟動");
                                $('#machine_status').text("啟動");
                                $("#status_machine").text(message.payloadString) ;
                                break;

                            case "2":
                                //$(dest).html("啟動");
                                $('#machine_status').text("遠端");
                                $("#status_machine").text(message.payloadString) ;
                                break;

                            case "3":
                                //$(dest).html("啟動");
                                $('#machine_status').text("本地");
                                $("#status_machine").text(message.payloadString) ;
                                break;
                        }  
                        break; 
                    case "#remote":
                        console.log("receive : " + message.destinationName + " "+ message.payloadString);
                        switch(message.payloadString)
                        {

                            case "1":
                                //$(dest).html("啟動");
                                $('#machine_status').text("遠端");
                                $("#status_machine").text(message.payloadString) 
                                    operationEnabled();
                                break;	
                            case "0":
                                //$(dest).html("啟動");
                                $('#machine_status').text("本地");
                                $("#status_machine").text(message.payloadString) 
                                    operationDisabled();
                                break;			 			 
                        }
                        break; 
                    case "#opmode":
                        switch(message.payloadString)
                        {
                            case "0":	  
                                $("#status_opmode").text(message.payloadString) ; 
                                $('#opmode_status').text("停止");
                                $("#opImage").attr("src","image/stop.png");
                                manualoperationEnabled(); 
                                break;	
                            case "1":
                                $("#status_opmode").text(message.payloadString) ; 

                                $('#opmode_status').text("健康舒適");
                                $("#opImage").attr("src","image/comfort1.png");
                                manuloperationDisabled();
                                break;		   
                            case "2":
                                $("#status_opmode").text(message.payloadString) ; 
                                $('#opmode_status').text("紫外線殺菌");
                                $("#opImage").attr("src","image/comfort2.png");
                                manuloperationDisabled();
                                break;
                            case "3":
                                $("#status_opmode").text(message.payloadString) ; 
                                $('#opmode_status').text("臭氧殺菌");
                                $("#opImage").attr("src","image/comfort500_333.png");
                                manuloperationDisabled();
                                break;						
                        }

                        break;  	  		   
                    case "#fan":
                        $("#status_fan").text(message.payloadString) ;
                        switch(message.payloadString)
                        {
                            case "0":
                                $('#fan_status').text("風扇停止");
                                $("#fanImage").attr("src","image/fan_off.png");
                                break;	
                            case "1":
                                $('#fan_status').text("風扇啟動");
                                $("#fanImage").attr("src","image/fan_on.png");
                                break;	
                        }
                        break;	 	   

                    case "#EP":
                        $("#status_EP").text(message.payloadString) ;
                        switch(message.payloadString)
                        {
                            case "1":
                                $("#EP_status").text("啟動");
                                $("#epImage").attr("src","image/ep_on.png");
                                break;			  
                            case "0":              
                            default:
                                $("#EP_status").text("停機");
                                $("#epImage").attr("src","image/ep_off.png");
                        }
                        break;
                    case "#UV_mode":   
                    case "#UV":
                        // 紫外線
                        $("#status_UV").text(message.payloadString) ;
                        switch(message.payloadString)
                        {
                            case "1":
                                $("#uv_status").text("啟動");
                                $("#uvImage").attr("src","image/uv_on.png");
                                break;
                            case "10":
                                $("#uv_status").text("運行10min");
                                $("#uvImage").attr("src","image/uv_on10.png");
                                break;
                            case "15":
                                $("#uv_status").text("運行15min");
                                $("#uvImage").attr("src","image/uv_on15.png");
                                break;	
                            case "30":
                                $("#uv_status").text("運行30min");
                                $("#uvImage").attr("src","image/uv_on30.png");
                                break;	
                            case "60":
                                $("#uvImage").attr("src","image/uv_on60.png");
                                $("#uv_status").text("運行60min");
                                break;																  				  
                            case "0":
                            default:
                                $("#uvImage").attr("src","image/uv_off.png");
                                $("#uv_status").text("停止");
                                break;	
                        }

                        break;
                    case "#o3_mode":	   
                    case "#ozone":
                        $("#status_ozone").text(message.payloadString) ;
                        switch(message.payloadString)
                        {
                            case "1":
                                $("#ozone_status").text("啟動");
                                $("#ozoneImage").attr("src","image/ozone_on.png");
                                break;
                            case "5":

                                $("#ozone_status").text("運行5min");
                                $("#ozoneImage").attr("src","image/ozone_on5.png");
                                break;  
                            case "10":
                                $("#ozone_status").text("運行10min");
                                $("#ozoneImage").attr("src","image/ozone_on10.png");
                                break; 
                            case "15":
                                $("#ozone_status").text("運行15min");
                                $("#ozoneImage").attr("src","image/ozone_on15.png");
                                break; 								
                            case "30":
                                $("#ozone_status").text("運行30min");
                                $("#ozoneImage").attr("src","image/ozone_on30.png");
                                break;			  
                            case "0":
                            default:
                                $("#ozone_status").text("停機");
                                $("#ozoneImage").attr("src","image/ozone_off.png");
                                break;	
                        }
                        break;
                    case "#ion":
                        $("#status_ion").text(message.payloadString) ;
                        switch(message.payloadString)
                        {
                            case "1":
                                $("#ionImage").attr("src","image/ion_on.png");
                                $("#ion_status").text("啟動");
                                //$("#ion").text("啟動");
                                break;			  
                            case "0":
                            default:
                                $("#ionImage").attr("src","image/ion_off.png");
                                $("#ion_status").text("停機");
                                //$("#ion").text("停機");
                                break;
                        }
                        break;	  

                    case "#buzzer":
                        $("#status_buzzer").text(message.payloadString) ;
                        switch(message.payloadString)
                        {
                            case "1":
                                {
                                    $("#buzzer_status").text("啟動");
                                    break;
                                }
                            case "0":
                            default:
                                $("#buzzer_status").text("停止");
                                break;
                        }
                        break;
                    case "#pwm_duty":
                        $("#status_pwm_duty").text(message.payloadString) ;
                        switch(message.payloadString)
                        {
                            case "250":
                                $('#fan_status').text("速度250");
                                $("#fanImage").attr("src","image/fan_on250.png");
                                break;					
                            case "400": 
                                $('#fan_status').text("速度400");
                                $("#fanImage").attr("src","image/fan_on400.png");
                                break;
                            case "550": 
                                $('#fan_status').text("速度550");
                                $("#fanImage").attr("src","image/fan_on550.png");
                                break
                            case "700": 
                                    $('#fan_status').text("速度700");
                                    $("#fanImage").attr("src","image/fan_on700.png");
                                    break;
                            case "850": 
                                    $('#fan_status').text("速度850");
                                    $("#fanImage").attr("src","image/fan_on850.png");
                                    break													
                        }
                        break;	  	   	  
                    default:
                        $(dest).html(message.payloadString);
                        $("#sensor").text(dest + ":" + message.payloadString) ;
                        break;  
                }
                break;
            } // status ===============================================
        case "sensor":
            {
                var dest = "#" + message.destinationName.substring(
                        message.destinationName.lastIndexOf("/") + 1);

                switch (dest)
                {
                    case ("#humidity"):
                        $("#sensor_humidity").text(message.payloadString + " (%)");
                        $("#humidity").text(message.payloadString + " (%)" );
                        break;
                    case ("#temperature"):
                        $("#sensor_temperature").text(message.payloadString + " (°C)") ;
                        $("#temperature").text(message.payloadString + " (°C)" ) ;
                        break;
                    case ("#prd_current"):
                        $("#sensor_prd_current").text(message.payloadString + " (A)") ;
                        $("#prd_current").text(message.payloadString + " (A)" ) ;
                        break;
                    case ("#motion") :
                        $("#sensor_motion").text(message.payloadString + "(人)") ;
                        $("#motion").text(message.payloadString + "(人)") ;
                        break;
                    case ("#PM25"):
                        $("#sensor_PM25").text(message.payloadString + "(μg/m3)") ;
                        $("#PM25").text(message.payloadString + "(μg/m3)"   ) ;
                        break;
                    case ("#fan_current"):
                        $("#sensor_fan_current").text(message.payloadString + " (A)") ;
                        $("#fan_current").text(message.payloadString + " (A)") ;
                        break;
                    case ("#UV"):
                        $("#sensor_UV").text(message.payloadString + "(μW/m2)") ;
                        $("#UV").text(message.payloadString + "(μW/m2)") ;
                        break;
                    case ("#ozone"):
                        $("#sensor_ozone").text(message.payloadString + " (ppb)") ;
                        $("#ozone").text(message.payloadString + " (ppb)") ;
                        break;
                    case ("#prd_current"):
                        $("#sensor_prd_current").text(message.payloadString + " (A)") ;
                        $("#pro_current").text(message.payloadString + " (A)") ;
                        break;	   
                    default:
                        $(dest).html(message.payloadString);
                        $("#sensor").text(dest + ":" + message.payloadString) ;
                        break; 
                }
            } // sensor ================================================
    } // messagetype 

}
*/

$(function(){
    var str="";
    var url = "server.iot.pwnass.com";
    var uri = "/XYCS/" + machine_sn + "/command/";
    alert("Hey, im here");
    /*
     * Fuck MQTT Over WS
     client = new Paho.MQTT.Client(
     "server.iot.pwnass.com", 
     Number(9001),
     "XYCS_" + parseInt(Math.random() * 100, 10)
     );
     client.onMessageArrived = onMessageArrived;
     client.connect({onSuccess: onConnect, useSSL: true});
     */

    $("#machine").change(function() {
        str=$("#machine").val();
        $('#machine_status').text=$("#machine").text();
        alert("Hey, im here");
        switch($("#machine").val())
        {
            case "1":
                // 啟動
                $("#machine option[value='1']").attr("disabled","disabled");
                $("#machine option[value='0']").removeAttr('disabled');
                /*
                   message = new Paho.MQTT.Message("1");
                   message.destinationName = "XYCS/1031/command/machine";
                   client.send(message);
                   console.log("send :" + message.destinationName +"  "+ message.payloadString);
                   */
                alert("Hey, im here");
                $.post(url+uri+"machine", {"data": 1}, function(data, status){
                    alert("Data: " + data + "\nStatus: " + status);
                });
                break;
            case "0":
                // 停機
                $("#machine option[value='0']").attr("disabled","disabled");
                $("#machine option[value='1']").removeAttr('disabled');
                /*
                   message = new Paho.MQTT.Message("0"); 
                   message.destinationName = "XYCS/1031/command/machine";
                   client.send(message);
                   console.log("send :" + message.destinationName +"  "+ message.payloadString); 
                   */
                $.post(url+uri+"machine", {"data": 1}, function(data, status){
                    alert("Data: " + data + "\nStatus: " + status);
                });
                break; 
        }

    });

    $("#opmode").change(function() {
        str=$("#opmode").val();
        $('#opmode_status').text=$("#opmode").text();
        switch($("#opmode").val()){ 
            case "0":
                //健康舒適#####################################
                $("#opImage").attr("src","image/stop.png");
                manualoperationEnabled();
                /*
                message = new Paho.MQTT.Message("0") ;
                message.destinationName = "XYCS/1031/command/opmode";
                client.send(message);
                console.log("send :" + message.destinationName +"  "+ message.payloadString);
                */
                break; 
            case "1":
                //健康舒適
                $("#opImage").attr("src","image/comfort1.png");
                manualoperationDisabled();
                /*
                message = new Paho.MQTT.Message("1") ;
                message.destinationName = "XYCS/1031/command/opmode";
                client.send(message);
                console.log("send :" + message.destinationName +"  "+ message.payloadString);
                */
                break; 
            case "2":
                // 3紫外線殺菌
                $("#opImage").attr("src","image/comfort2.png");
                manualoperationDisabled();
                /*
                message = new Paho.MQTT.Message("2"); 
                message.destinationName = "XYCS/1031/command/opmode";
                client.send(message);
                console.log("send :" + message.destinationName +"  "+ message.payloadString);
                */
                break;
            case "3":
                // 臭氧殺菌
                $("#opImage").attr("src","image/comfort500_333.png");
                manualoperationDisabled();
                /*
                message = new Paho.MQTT.Message("3") ;
                message.destinationName = "XYCS/1031/command/opmode";
                client.send(message); 
                console.log("send :" + message.destinationName +"  "+ message.payloadString);
                */
                break;

        }
    });

    //  fan 風扇 
    $("#fan").change(function() {
        $('#fan_status').text=$("#fan").text();
        switch($("#fan").val()){
            case "0":
                {
                    //0:風扇停止
                    message = new Paho.MQTT.Message("0");
                    $("#fanImage").attr("src","image/fan_off.png");
                    /*
                    message.destinationName = "XYCS/1031/command/fan";
                    client.send(message);
                    */
                    //  console.log("send :" + message.destinationName +"  "+ message.payloadString);
                    break;
                }
            case "1":
                {
                    //1:風扇啟動

                    $("#fanImage").attr("src","image/fan_0n250.png");
                    /*
                    message = new Paho.MQTT.Message("1"); 
                    message.destinationName = "XYCS/1031/command/fan";
                    client.send(message);
                    message = new Paho.MQTT.Message("250"); 
                    message.destinationName = "XYCS/1031/command/pwm_duty";
                    client.send(message);
                    */
                    // console.log("send :" + message.destinationName +"  "+ message.payloadString);

                    break;
                } 
            case "250":
                {
                    //"速度250
                    //$("#fan li:eq(1)").attr('disabled',true)
                    $("#fanImage").attr("src","image/fan_on250.png");
                    /*
                    message = new Paho.MQTT.Message("250"); 
                    message.destinationName = "XYCS/1031/command/pwm_duty";
                    client.send(message);
                    */
                    //console.log("send :" + message.destinationName +"  "+ message.payloadString);
                    break;
                }
            case "400":
                {  // 速度400
                    //$("#fan li:eq(1)").attr('disabled',true)
                    $("#fanImage").attr("src","image/fan_on400.png");
                    /*
                    message = new Paho.MQTT.Message("400"); 
                    message.destinationName = "XYCS/1031/command/pwm_duty";
                    client.send(message);
                    */
                    // console.log("send :" + message.destinationName +"  "+ message.payloadString);
                    break;
                }
            case "550":
                //"速度550")
                {
                    //$("#fan li:eq(1)").attr('disabled',true)
                    $("#fanImage").attr("src","image/fan_on550.png");
                    /*
                    message = new Paho.MQTT.Message("550"); 
                    message.destinationName = "XYCS/1031/command/pwm_duty";
                    client.send(message);
                    */
                    // console.log("send :" + message.destinationName +"  "+ message.payloadString);
                    break;
                }
            case "700":
                {
                    //"速度700"
                    //$("#fan li:eq(1)").attr('disabled',true)
                    $("#fanImage").attr("src","image/fan_on700.png");
                    /*
                    message = new Paho.MQTT.Message("700"); 
                    message.destinationName = "XYCS/1031/command/pwm_duty";
                    client.send(message);
                    */
                    // console.log("send :" + message.destinationName +"  "+ message.payloadString);
                    break;
                } 
            case "850":
                {
                    //"速度850"){
                    //$("#fan li:eq(1)").attr('disabled',true)
                    $("#fanImage").attr("src","image/fan_on850.png");
                    /*
                    message = new Paho.MQTT.Message("850"); 
                    message.destinationName = "XYCS/1031/command/pwm_duty";
                    client.send(message); 
                    */
                    // console.log("send :" + message.destinationName +"  "+ message.payloadString);
                    break;   
                }
                }
                // console.log('aaaaaaa')
        });

        $("#EP").change(function() {
            $('#EP_status').text=$("#EP").text();
            //console.log(str);
            switch($("#EP").val()){	
                case "1":
                    {
                        // $("#machine:2").attr('disabled','disabled')
                        $("#epImage").attr("src","image/ep_on.png");
                        /*
                        message = new Paho.MQTT.Message("1");
                        message.destinationName = "XYCS/1031/command/EP";
                        client.send(message);
                        */
                        break;
                    }
                case "0":
                    {
                        //"停機"){
                        $("#epImage").attr("src","image/ep_off.png");
                        /*
                        message = new Paho.MQTT.Message("0") ;
                        message.destinationName = "XYCS/1031/command/EP";
                        client.send(message);
                        */
                        break;
                    }
                    }
                    // console.log('aaaaaaa')
            });

            $("#s-UV").change(function() {
                //$('#UV_status').text=$("#UV").text();
                switch($("#s-UV").val()){
                    case "0":
                        {
                            //0:停機
                            //$("#uvImage").attr("src","")
                            $("#uv_status").text="停機" ; 
                            $("#uvImage").attr("src","image/uv_off.png");
                            /*
                            message = new Paho.MQTT.Message("0");
                            message.destinationName = "XYCS/1031/command/UV";
                            client.send(message);
                            */
                            // console.log("send :" + message.destinationName +"  "+ message.payloadString);
                            break;
                        }
                    case "1":
                        {
                            //1啟動
                            //$("#fan li:eq(1)").attr('disabled',true)
                            //$("#uvImage").attr("src","image/uv_ozone_lamp.png");

                            $("#uv_status").text="啟動";
                            $("#uvImage").attr("src","image/uv_on.png");
                            /*
                            message = new Paho.MQTT.Message("1"); 
                            message.destinationName = "XYCS/1031/command/UV";
                            client.send(message);
                            */
                            // console.log("send :" + message.destinationName +"  "+ message.payloadString);
                            break;
                        }
                    case "10":
                        {  // 運行10min
                            //$("#fan li:eq(1)").attr('disabled',true)
                            //$("#uvImage").attr("src","image/uv_ozone_lamp.png");
                            $("#uv_status").text= "運行10min";
                            $("#uvImage").attr("src","image/uv_on10.png");
                            /*
                            message = new Paho.MQTT.Message("10"); 
                            message.destinationName = "XYCS/1031/command/UV";
                            client.send(message);
                            console.log("send :" + message.destinationName +"  "+ message.payloadString);
                            */
                            break;
                        }
                    case "15":
                        // 運行15min)
                        {
                            //$("#fan li:eq(1)").attr('disabled',true)
                            $("#uv_status").text= "運行15min";
                            $("#uvImage").attr("src","image/uv_on15.png");
                            /*
                            message = new Paho.MQTT.Message("15"); 
                            message.destinationName = "XYCS/1031/command/UV";
                            client.send(message);
                            */
                            break;
                        }
                    case "30":
                        {
                            // 運行30min
                            //$("#fan li:eq(1)").attr('disabled',true)
                            $("#uv_status").text="運行30min";
                            $("#uvImage").attr("src","image/uv_on30.png");
                            /*
                            message = new Paho.MQTT.Message("30"); 
                            message.destinationName = "XYCS/1031/command/UV";
                            client.send(message);
                            */
                            break;
                        } 
                    case "60":
                        {
                            // 運行60 min
                            //$("#fan li:eq(1)").attr('disabled',true)
                            $("#uv_status").text="運行60min";
                            $("#uvImage").attr("src","image/uv_on60.png");
                            /*
                            message = new Paho.MQTT.Message("60"); 
                            message.destinationName = "XYCS/1031/command/UV";
                            client.send(message); 
                            */
                            break;   
                        }
                        break; 
                }
                // console.log('aaaaaaa')

            }); 

            //臭氧O3
            $("#s-ozone").change(function() {
                $('#ozone_status').text=$("#s-ozone").text();
                switch($("#s-ozone").val()){
                    case "0":
                        //0:風扇停止
                        $("#ozone_status").text="運行60min";
                        $("#ozoneImage").attr("src","image/ozone_off.png");
                        /*
                        message = new Paho.MQTT.Message("0");
                        message.destinationName = "XYCS/1031/command/ozone";
                        client.send(message);
                        */
                        break;
                    case "1":
                        //"速度250
                        //$("#fan li:eq(1)").attr('disabled',true)
                        $("#ozone_status").text="啟動";
                        $("#ozoneImage").attr("src","image/ozone_on.png");
                        /*
                        message = new Paho.MQTT.Message("1"); 
                        message.destinationName = "XYCS/1031/command/ozone";
                        client.send(message);
                        */
                        break;
                    case "5":
                        // 速度400
                        //$("#fan li:eq(1)").attr('disabled',true)
                        $("#ozone_status").text="運行5min";
                        $("#ozoneImage").attr("src","image/ozone_on.png");
                        /*
                        message = new Paho.MQTT.Message("5"); 
                        message.destinationName = "XYCS/1031/command/ozone";
                        client.send(message);
                        */
                        break;

                    case "10":
                        //$("#fan li:eq(1)").attr('disabled',true)
                        $("#ozone_status").text="運行10min";
                        $("#ozoneImage").attr("src","image/ozone_on.png");
                        /*
                        message = new Paho.MQTT.Message("10"); 
                        message.destinationName = "XYCS/1031/command/ozone";
                        client.send(message);
                        */
                        break;
                    case "15":
                        //"速度700"
                        //$("#fan li:eq(1)").attr('disabled',true)
                        $("#ozone_status").text="運行15min";
                        $("#ozoneImage").attr("src","image/ozone_on.png");
                        /*
                        message = new Paho.MQTT.Message("15"); 
                        message.destinationName = "XYCS/1031/command/ozone";
                        client.send(message);
                        */
                        break;

                    case "30":

                        //"速度850"){
                        //$("#fan li:eq(1)").attr('disabled',true)
                        $("#ozone_status").text="運行30min");
                        $("#ozoneImage").attr("src","image/ozone_on.png");
                        /*
                        message = new Paho.MQTT.Message("30"); 
                        message.destinationName = "XYCS/1031/command/ozone";
                        client.send(message); 
                        break;   
                }
                // console.log('aaaaaaa')
                });

                $("#ion").change(function() {
                    $('#ion_status').text=$("#ion").text();
                    switch ($("#ion").val()){	
                        case "1":
                            {
                                $("#ionImage").attr("src","image/ion_on.png");
                                /*
                                message = new Paho.MQTT.Message("1");
                                message.destinationName = "XYCS/1031/command/ion";
                                client.send(message);
                                console.log("send   " + message.destinationName + " " + message.payloadString );
                                */
                                break;
                            }
                        case "0":
                            {
                                //"停機"){
                                $("#ionImage").attr("src","image/ion_off.png");
                                /*
                                message = new Paho.MQTT.Message("0") ;
                                message.destinationName = "XYCS/1031/command/ion";

                                client.send(message);
                                console.log("send   " + message.destinationName + " " + message.payloadString );
                                */
                                break;
                            }
                            }

                    });
                    /*
                       $("#buzzer-stop").prop("disabled", true);
                       $("#buzzer-stop").on("click", function(){
                       $(this).prop("disabled", true);
                       $("#buzzer-start").removeAttr("disabled");
                       $("#buzzerImage").attr("src","image/buzzer_off.png");
                       $("#buzzer").text("停止");
                       message = new Paho.MQTT.Message("0");
                       message.destinationName = "XYCS/1031/command/buzzer";
                       client.send(message);
                       });
                       $("#buzzer-start").on("click", function(){
                       $(this).prop("disabled", true);
                       $("#buzzer-stop").removeAttr("disabled");
                       $("#buzzerImage").attr("src","image/buzzer_on.png");
                       $("#buzzer").text("停止");
                       message = new Paho.MQTT.Message("1");
                       message.destinationName = "XYCS/1031/command/buzzer";
                       client.send(message);
                       });
                       $("#pwm_duty-stop").prop("disabled", true);
                       $("#pwm_duty-stop").on("click", function(){
                       $(this).prop("disabled", true);
                       $("#pwm_duty-start").removeAttr("disabled");
                       $("#pwmImage").attr("src","image/pwm_off.png");
                       $("#pwm_duty").text("停止")
                       message = new Paho.MQTT.Message("0");
                       message.destinationName = "XYCS/1031/command/pwm_duty";
                       client.send(message);
                       });
                       $("#pwm_duty-start").on("click", function(){
                       $(this).prop("disabled", true); pwm_duty
                       $("#pwm_duty-stop").removeAttr("disabled");
                       $("#pwmImage").attr("src","image/pwm_ON.png");
                       $("#pwm_duty").text("啟動");
                       message = new Paho.MQTT.Message("1");
                       message.destinationName = "XYCS/1031/command/pwm_duty";
                       client.send(message);
                    // 250 400 550 700 850
                    });
                    */

                });
                function  operationEnabled(){
                    $("#machine option[value='0']").removeAttr('disabled');
                    $("#machine option[value='1']").removeAttr('disabled');	
                    $("#machine option[value='2']").removeAttr('disabled');
                    $("#machine option[value='3']").removeAttr('disabled');

                    $("#opmode option[value='0']").removeAttr('disabled');
                    $("#opmode option[value='1']").removeAttr('disabled');
                    $("#opmode option[value='2']").removeAttr('disabled');
                    $("#opmode option[value='3']").removeAttr('disabled');

                    $("#fan option[value='0']").removeAttr('disabled');
                    $("#fan option[value='1']").removeAttr('disabled');
                    $("#fan option[value='250']").removeAttr('disabled');
                    $("#fan option[value='400']").removeAttr('disabled');
                    $("#fan option[value='550']").removeAttr('disabled');
                    $("#fan option[value='700']").removeAttr('disabled');
                    $("#fan option[value='850']").removeAttr('disabled');
                    $("#fan option[value='850']").removeAttr('disabled');

                    $("#EP option[value='0']").removeAttr('disabled');
                    $("#EP option[value='1']").removeAttr('disabled');

                    $("#ion option[value='0']").removeAttr('disabled');
                    $("#ion option[value='1']").removeAttr('disabled'); 

                    $("#s-UV option[value='0']").removeAttr('disabled'); 
                    $("#s-UV option[value='1']").removeAttr('disabled'); 
                    $("#s-UV option[value='10']").removeAttr('disabled'); 
                    $("#s-UV option[value='15']").removeAttr('disabled'); 
                    $("#s-UV option[value='30']").removeAttr('disabled'); 
                    $("#s-UV option[value='60']").removeAttr('disabled'); 

                    $("#s-ozone option[value='0']").removeAttr('disabled'); 
                    $("#s-ozone option[value='1']").removeAttr('disabled');
                    $("#s-ozone option[value='5']").removeAttr('disabled');
                    $("#s-ozone option[value='10']").removeAttr('disabled');
                    $("#s-ozone option[value='15']").removeAttr('disabled');
                    $("#s-ozone option[value='30']").removeAttr('disabled');
                }
                function  operationDisabled(){

                    $("#machine option[value='0']").attr("disabled","disabled");	
                    $("#machine option[value='1']").attr("disabled","disabled");
                    $("#machine option[value='2']").attr("disabled","disabled");
                    $("#machine option[value='3']").attr("disabled","disabled");

                    $("#opmode option[value='0']").attr("disabled","disabled");
                    $("#opmode option[value='1']").attr("disabled","disabled");
                    $("#opmode option[value='2']").attr("disabled","disabled");
                    $("#opmode option[value='3']").attr("disabled","disabled");

                    $("#fan option[value='0']").attr("disabled","disabled");
                    $("#fan option[value='1']").attr("disabled","disabled");
                    $("#fan option[value='250']").attr("disabled","disabled");
                    $("#fan option[value='400']").attr("disabled","disabled");
                    $("#fan option[value='550']").attr("disabled","disabled");
                    $("#fan option[value='700']").attr("disabled","disabled");
                    $("#fan option[value='850']").attr("disabled","disabled");	

                    $("#EP option[value='0']").attr("disabled","disabled");
                    $("#EP option[value='1']").attr("disabled","disabled");

                    $("#ion option[value='0']").attr("disabled","disabled");
                    $("#ion option[value='1']").attr("disabled","disabled");

                    $("#s-UV option[value='0']").attr("disabled","disabled"); 
                    $("#s-UV option[value='1']").attr("disabled","disabled");
                    $("#s-UV option[value='10']").attr("disabled","disabled");
                    $("#s-UV option[value='15']").attr("disabled","disabled");
                    $("#s-UV option[value='30']").attr("disabled","disabled"); 
                    $("#s-UV option[value='60']").attr("disabled","disabled");

                    $("#s-ozone option[value='0']").attr("disabled","disabled");
                    $("#s-ozone option[value='1']").attr("disabled","disabled");
                    $("#s-ozone option[value='5']").attr("disabled","disabled");
                    $("#s-ozone option[value='10']").attr("disabled","disabled");
                    $("#s-ozone option[value='15']").attr("disabled","disabled");
                    $("#s-ozone option[value='30']").attr("disabled","disabled");

                };


                function  manualoperationEnabled(){



                    $("#fan option[value='1']").removeAttr('disabled');
                    $("#fan option[value='0']").removeAttr('disabled');
                    $("#fan option[value='250']").removeAttr('disabled');
                    $("#fan option[value='400']").removeAttr('disabled');
                    $("#fan option[value='550']").removeAttr('disabled');
                    $("#fan option[value='700']").removeAttr('disabled');
                    $("#fan option[value='850']").removeAttr('disabled');
                    $("#fan option[value='850']").removeAttr('disabled');

                    $("#EP option[value='0']").removeAttr('disabled');
                    $("#EP option[value='1']").removeAttr('disabled');

                    $("#ion option[value='0']").removeAttr('disabled');
                    $("#ion option[value='1']").removeAttr('disabled'); 

                    $("#s-UV option[value='0']").removeAttr('disabled'); 
                    $("#s-UV option[value='1']").removeAttr('disabled'); 
                    $("#s-UV option[value='10']").removeAttr('disabled'); 
                    $("#s-UV option[value='15']").removeAttr('disabled'); 
                    $("#s-UV option[value='30']").removeAttr('disabled'); 
                    $("#s-UV option[value='60']").removeAttr('disabled'); 

                    $("#s-ozone option[value='0']").removeAttr('disabled'); 
                    $("#s-ozone option[value='1']").removeAttr('disabled');
                    $("#s-ozone option[value='5']").removeAttr('disabled');
                    $("#s-ozone option[value='10']").removeAttr('disabled');
                    $("#s-ozone option[value='15']").removeAttr('disabled');
                    $("#s-ozone option[value='30']").removeAttr('disabled');
                }
                function  manualoperationDisabled(){


                    $("#fan option[value='1']").attr("disabled","disabled"); 
                    $("#fan option[value='0']").attr("disabled","disabled");
                    $("#fan option[value='250']").attr("disabled","disabled");
                    $("#fan option[value='400']").attr("disabled","disabled");
                    $("#fan option[value='550']").attr("disabled","disabled");
                    $("#fan option[value='700']").attr("disabled","disabled");
                    $("#fan option[value='850']").attr("disabled","disabled");	

                    $("#EP option[value='0']").attr("disabled","disabled");
                    $("#EP option[value='1']").attr("disabled","disabled");

                    $("#ion option[value='0']").attr("disabled","disabled");
                    $("#ion option[value='1']").attr("disabled","disabled");

                    $("#s-UV option[value='0']").attr("disabled","disabled"); 
                    $("#s-UV option[value='1']").attr("disabled","disabled");
                    $("#s-UV option[value='10']").attr("disabled","disabled");
                    $("#s-UV option[value='15']").attr("disabled","disabled");
                    $("#s-UV option[value='30']").attr("disabled","disabled"); 
                    $("#s-UV option[value='60']").attr("disabled","disabled");

                    $("#s-ozone option[value='0']").attr("disabled","disabled");
                    $("#s-ozone option[value='1']").attr("disabled","disabled");
                    $("#s-ozone option[value='5']").attr("disabled","disabled");
                    $("#s-ozone option[value='10']").attr("disabled","disabled");
                    $("#s-ozone option[value='15']").attr("disabled","disabled");
                    $("#s-ozone option[value='30']").attr("disabled","disabled");

                };
