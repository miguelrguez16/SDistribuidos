import com.rabbitmq.client.ConnectionFactory;
import com.rabbitmq.client.Connection;
import com.rabbitmq.client.Channel;
//Channel -> canal para mandar comandos AMQP
//Connection -> 
//ConnectionFactory -> 
//COMPILAR javac -cp rabbitmq-client.jar Send.java
//EJECUTAR java -cp .:rabbitmq-client.jar Send
public class Send {
    private final static String QUEUE_NAME = "hello";
    public static void main(String[] argv) throws Exception {
        //Creacion del canal
        ConnectionFactory factory = new ConnectionFactory();
        factory.setHost("localhost"); //Host donde esta el servidor rabit
        Connection connection = factory.newConnection();
        Channel channel = connection.createChannel();
    
        //creacion de la cola
        channel.queueDeclare(QUEUE_NAME, false, false, false, null);
        String message = "Hello World!";
                //AMQ no sabe de tipos de datos, solo espera una secuencia de datos
                                //uso del intercambiador por defecto por eso ""
                                
        channel.basicPublish("", QUEUE_NAME, null, message.getBytes());
        System.out.println(" [x] Sent '" + message + "'");

        channel.close();
        connection.close(); 
        //se van a ir turnando por los consumidores, van por orden
    }
  }