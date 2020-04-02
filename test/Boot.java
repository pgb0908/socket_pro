package test;

public class Boot {

    public static void main(String[] args) {
        // TODO Auto-generated method stub
        //Executors.newSingleThreadExecutor().execute(new Server("localhost", 10000));
        //Executors.newSingleThreadExecutor().execute(new Client("127.0.0.1", 10000));
        
        
        Server server = new Server("localhost", 10000);
        Client client = new Client("localhost", 10000);
        
        
        
        //server.run();
        new Thread(server).start();
        
        
        
        client.run();
        //new Thread(client).start();
    }

}
