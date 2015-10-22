import java.io.BufferedReader;
import java.io.PrintWriter;
import java.io.InputStreamReader;
import java.lang.StringBuffer;
import java.net.Socket;
import java.io.IOException;

class HotelGatewayThread extends HotelDisplayLogic implements Runnable{
	private Socket clientSocket;

	public HotelGatewayThread(Socket clientSocket, Hotel hotelObject){
		this.clientSocket = clientSocket;
		this.hotelObject = hotelObject;
	}

	public void run(){
		PrintWriter out;
		BufferedReader in;
		String fullInput = "";

	    try{
	    	out = new PrintWriter(clientSocket.getOutputStream(), true);
			in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
		}
		catch(IOException e){
			e.printStackTrace();
			return;
		}

		while(true){
			out.print("hotelgw>");
			out.flush();

			try{
				fullInput = in.readLine();
			}
			catch(IOException e){
				e.printStackTrace();
				closeConnection(out, clientSocket);
				return;
			}

			if(fullInput == null){	//remote has closed connection gracefully
				closeConnection(out, clientSocket);
				return;
			}

			if(!parseAndProcessInput(out, fullInput)){	//returns false if quit
				closeConnection(out, clientSocket);
				return;
			}
		}
	}

	//returns false if connection should be closed, true otherwise
	private boolean parseAndProcessInput(PrintWriter out, String fullInput){
		int action = 0;
		int roomType = 0;
		String guestName = "";
		String[] splittedInput = new String[1];

		splittedInput = fullInput.split("\\s+", 3);

		if(splittedInput[0].equals("")){
			return true;
		}

		char command = splittedInput[0].charAt(0);

		switch(command){
			case 'l':
				action = 1;
			break;
			case 'b':
				if(splittedInput.length != 3){
					action = -1;
					out.println("Booking requires a room type and a guest name");
				}
				else{
    				try{
						roomType = Integer.parseInt(splittedInput[1]);
						guestName = splittedInput[2];
						action = 2;
					}
					catch(NumberFormatException e){
						out.println("Could not convert room type to integer");
     					action = -1;
    				}
				}

			break;
			case 'g':
				action = 3;
			break;
			case 'q':
				action = 4;
			break;
			case 'h':
			default:
			break;
		}
	
		if(action == 4){		//quit
			return false;
		}
		else if(action >= 0){
		    performAction(out, action, roomType, guestName);
		}

		return true;
	}

	private void closeConnection(PrintWriter out, Socket clientSocket){
		try{
			out.flush();
			out.close();
			clientSocket.close();
		}
		catch(IOException e){
			e.printStackTrace();
		}
	}

	protected void printHelp(PrintWriter out){
		String help = "This program can be used by the attendees of a conference to make room reservations. Reservation dates are fixed, so book with care.\n" +
			"\n" +
			"Usage\n" +
			"\n" +
			"Help:\n" +
			"	h\n" +
			" 	Displays this help message.\n" +
			"List available rooms:\n" +
			" 	l\n" +
			" 	Lists the number of rooms available in each price range.\n" +
			"Book a room:\n" +
			"	b <type> Guest Name\n" +
			"	Books a room of the specified type (if available) on the given name.\n" +
			"List guests:\n" +
			"	g\n" +
			"	Lists the names of all registered guests.\n" +
			"Quit:\n" +
			"	q\n" +
			"	Quits the telnet session.\n";

		out.printf("%s", help);
	}
}