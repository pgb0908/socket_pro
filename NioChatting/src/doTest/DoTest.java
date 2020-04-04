package doTest;

import Server.Server;
import Client.Client;

public class DoTest {

    public static void main(String[] args) {
        // TODO Auto-generated method stub
        
        Server server = new Server();
        server.start();
        
        
        //System.out.println("Client is initing...");
        Client client = new Client();
        client.start();

    }

}
