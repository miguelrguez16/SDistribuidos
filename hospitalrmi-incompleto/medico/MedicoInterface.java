package medico;
import java.rmi.Remote;
import java.rmi.RemoteException;

/*
Interfaz remoto del Medico que ha de hacer público los metodos siguientes: 
   quirofanoConcedido para que Coordinacion  pueda invocarlo vía RMI
   equipoConcedido para que Coordinacion pueda invocarlo via RMI
*/
public interface MedicoInterface extends Remote {
    // A RELLENAR
}
