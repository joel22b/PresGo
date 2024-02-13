def generate_cylinder_points(self, radius, height, num_points):
    theta = np.linspace(0, 2 * np.pi, num_points)
    t = np.linspace(0, 1, num_points)

    theta, t = np.meshgrid(theta, t)

    x = radius * np.cos(theta)
    y = radius * np.sin(theta)
    z = height * t

    return x, y, z

  def draw_cylinder(self, radius,height,num_points):
    x,y,z =self.generate_cylinder_points(radius,height, num_points)
    # Combine base points and lines to form the cone

    points = np.array([x,y,z])
    # Draw the cone
    cone_item = gl.GLLinePlotItem(pos=np.column_stack((x.flatten(), y.flatten(), z.flatten())), color=(1, 0, 0, 1), width=3)

    self.w.addItem(cone_item)
  def is_point_inside_cylinder(self,points, radius, height):
    x, y, z = points
    # Check circular cross-section condition
    in_circle = x**2 + y**2 <= radius**2
    
    # Check height condition
    in_height_range = 0 <= z <= height
    
    # Point is inside the cylinder if both conditions are met
    return in_circle and in_height_range