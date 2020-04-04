package Client;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;
import java.nio.charset.Charset;
import java.util.Scanner;

public class WriteToServer extends Thread {

    private SocketChannel sc;
    private ByteBuffer buffer;

    WriteToServer(SocketChannel sc, ByteBuffer buffer) {
        this.sc = sc;
        this.buffer = buffer;
    }

    public void run() {
        while(true) {
            writeToServer();
        }

    }

    private void writeToServer() {
        Scanner scanner = new Scanner(System.in);;
        String msg = scanner.next();
        
        System.out.println(msg);

        buffer.clear();
        //Charset charset = Charset.forName("UTF-8");
        //buffer = charset.encode(msg);
        
        buffer.put(msg.getBytes());
        
        //printState(buffer);
        
        buffer.flip();
        
        //printState(buffer);
        try {
            sc.write(buffer);
        } catch (IOException e) {
            e.printStackTrace();
        }
        
    }

    private void printState(ByteBuffer buffer2) {
        System.out.print("\tposition: " + buffer.position() + ", ");
        System.out.print("\tlimit: " + buffer.limit() + ", ");
        System.out.println("\tcapacity: " + buffer.capacity());
        
    }
}
