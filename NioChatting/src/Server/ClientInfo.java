package Server;

import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;

public class ClientInfo {
    
    private static final int BUF_SIZE = 256;
    
    private int clientNumber;
    private String clientID;
    SocketChannel sc;
    
    ByteBuffer readBuffer = ByteBuffer.allocate(BUF_SIZE);
    ByteBuffer writeBuffer = ByteBuffer.allocate(BUF_SIZE);
    
    
    ClientInfo(int clientNumber, String clientID, SocketChannel sc) {
        this.setClientNumber(clientNumber);
        this.setClientID(clientID);
        this.sc = sc;
    }


    public int getClientNumber() {
        return clientNumber;
    }


    public void setClientNumber(int clientNumber) {
        this.clientNumber = clientNumber;
    }


    public String getClientID() {
        return clientID;
    }


    public void setClientID(String clientID) {
        this.clientID = clientID;
    }

}
