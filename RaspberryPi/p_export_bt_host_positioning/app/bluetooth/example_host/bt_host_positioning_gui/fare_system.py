from uuid import UUID

invalid_fare = UUID(int=0)

class FareSystem:
    db = {}
    farePrice = 3.00
    fareError = -100
    fareNotFound = -99

    def __init__(self):
        # Load initial values
        self.db[UUID(int=1)] = 222.22
        self.db[UUID(int=2)] = 4.04
        self.db[UUID(int=3)] = 33.33
        self.db[UUID(int=4)] = 0.67
        self.db[UUID(int=5)] = 10.10

    def validate_fare(self, fareUuid: UUID):
        if fareUuid == invalid_fare:
            return (False, self.fareError)
        if fareUuid in self.db:
            if self.db[fareUuid] >= self.farePrice:
                self.db[fareUuid] -= self.farePrice
                return (True, self.db[fareUuid])
            else:
                return (False, self.db[fareUuid])
        return (False, self.fareNotFound)
