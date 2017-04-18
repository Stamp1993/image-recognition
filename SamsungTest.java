import java.io.IOException;

import javax.imageio.ImageIO;

import java.io.File;
import java.awt.image.BufferedImage;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.stream.Stream;

class SamsungTest {
	
	
	public static void main(String[] args) throws IOException{
		try(Stream<Path> paths = Files.walk(Paths.get("./clocks"))) {
		    paths.forEach(filePath -> {
		        if (Files.isRegularFile(filePath)) {
		            System.out.println(filePath);
		            
		            String file = filePath.toString();
		            System.out.println(file);
		            write(file);
		        }
		    });
		}
		
		try(Stream<Path> paths = Files.walk(Paths.get("./crocodiles"))) {
		    paths.forEach(filePath -> {
		        if (Files.isRegularFile(filePath)) {
		            System.out.println(filePath);
		           
		            String file = filePath.toString();
		            System.out.println(file);
		            write(file);
		        }
		    });
		} 
		
		
		//write("35.png");
	}
	
	public static void write(String IMG){
		BufferedImage img;
		String out = IMG.substring(0, IMG.length()-3) + "txt";
	    try {
	        img = ImageIO.read(new File(IMG));

	        int[][] pixelData = new int[img.getHeight() * img.getWidth()][3];
	        int[] rgb;

	        FileWriter fw = new FileWriter(out);
			BufferedWriter bw = new BufferedWriter(fw);
			
	        int counter = 0;
	        for(int i = 0; i < img.getWidth(); i++){
	            for(int j = 0; j < img.getHeight(); j++){
	                rgb = getPixelData(bw, img, i, j);

	                for(int k = 0; k < rgb.length; k++){
	                    pixelData[counter][k] = rgb[k];
	                }

	                counter++;
	            }
	        }
	        bw.close();

	    } catch (IOException e) {
	        e.printStackTrace();
	    }
	   

	}
	
	private static int[] getPixelData(BufferedWriter bw, BufferedImage img, int x, int y) throws IOException {
		
		
		
		int argb = img.getRGB(x, y);

		int rgb[] = new int[] {
		    (argb >> 16) & 0xff, //red
		    (argb >>  8) & 0xff, //green
		    (argb      ) & 0xff  //blue
		};

		System.out.println("rgb: " + rgb[0] + " " + rgb[1] + " " + rgb[2]);
		bw.write("" + rgb[0] + " " + rgb[1] + " " + rgb[2]);
		bw.newLine();
		return rgb;
		}
}