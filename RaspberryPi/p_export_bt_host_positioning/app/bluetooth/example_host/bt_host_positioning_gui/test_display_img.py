from PIL import Image

def display_image(image_path):
    try:
        img = Image.open(image_path)
        img.show()
    except IOError as e:  
        print(f"Error: Cannot open the specified image. Exception: {e}")

def main():
    fare_responses = { 
        True: "Images/check.png",
        False: "Images/x.png"
    }
    fare_valid_state = None
    
    while True:
        user_input = input("Do you want to display the check image? (yes/no/exit): ")
        
        if user_input.lower() == "yes":
            fare_valid_state = True
        elif user_input.lower() == "no":
            fare_valid_state = False
        elif user_input.lower() == "exit":
            return
        else:
            fare_valid_state = None
            print('Invalid input. Please enter "yes", "no", or "exit".')

        if fare_valid_state in fare_responses:
            display_image(fare_responses[fare_valid_state])

if __name__ == "__main__":
    main()