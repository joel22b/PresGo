from uuid import UUID

invalid_fare = UUID("00000000000000000000000000000000")

class FareSystem:
    db = {}
    farePrice = 3.00
    fareError = -100
    fareNotFound = -99

    def __init__(self):
        # Load initial values
        self.db[UUID("00000000000000000000000000000001")] = 222.22
        self.db[UUID("00000000000000000000000000000002")] = 4.04
        self.db[UUID("00000000000000000000000000000003")] = 33.33
        self.db[UUID("00000000000000000000000000000004")] = 0.67
        self.db[UUID("00000000000000000000000000000005")] = 10.10

    def validate_fare(self, fareUuid: UUID):
        if fareUuid == invalid_fare:
            return (False, self.fareError)
        
        if fareUuid in self.db:
            if self.db[fareUuid] > 0:
                self.db[fareUuid] -= self.farePrice
                return (True, self.db[fareUuid])
            else:
                return (False, self.db[fareUuid])
        return (False, self.fareNotFound)
