from uuid import UUID
class Fare:
    
  
    def __init__(self) -> None:
      self.valid_fares=  {UUID( "a22222222222222222222222222222ff"):True}

    def check_valid(self,uuid:UUID):
       if uuid in self.valid_fares:
          return self.valid_fares[uuid]
       else:
          return False
    

