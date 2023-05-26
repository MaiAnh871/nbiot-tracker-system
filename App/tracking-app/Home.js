import { StyleSheet, Text, View, Pressable, Dimensions, Image } from 'react-native';
import { Auth } from 'aws-amplify';
import { useNavigation } from '@react-navigation/native';

const { width } = Dimensions.get('window');


const Home = () => {
    const navigation = useNavigation();

    const navigateToQRScanner = () => {
      navigation.navigate('QRScanner');
    };

    const signOut = async () => {
      try {
        await Auth.signOut({ global: true });
      } catch (error) {
        console.log('Error signing out: ', error);
      }
    };

    return (
      <View style={styles.container}>
        <View style={styles.header}>
          <Text style={styles.headerText}>DANH SÁCH PHƯƠNG TIỆN</Text>
        </View>
        <Pressable style={styles.button} onPress={() => signOut()}>
          <Text style={styles.buttonText}>Sign out</Text>
        </Pressable>
        <Pressable style={styles.addButton} onPress={navigateToQRScanner}>
          <Image source={require('./plus_icon.png')} style={styles.plusIcon} />
        </Pressable>
      </View>
    );
  };

const styles = StyleSheet.create({
    container: {
      flex: 1,
      backgroundColor: '#fff',
      alignItems: 'center',
      justifyContent: 'flex-start', // Align items from top to bottom
      width: width,
      paddingTop: 10, // Add paddingTop to create space at the top
    },
    header: {
      display: 'flex',
      flexDirection: 'row',
      justifyContent: 'space-between',
      paddingHorizontal: 20, // Update padding to only horizontal
      paddingVertical: 20,
      width: width,
      alignItems: 'center',
      marginTop: 10, // Add marginTop to move the header to the top
    },
    headerText: {
      fontSize: 28,
      fontWeight: 'bold',
    },
    button: {
      position: 'absolute',
      bottom: 20,
      right: 20,
      backgroundColor: '#ff9900',
      padding: 10,
      borderRadius: 6,
    },
    buttonText: {
      color: '#fff',
      fontSize: 18,
    },
    addButton: {
      position: 'absolute',
      bottom: 20,
      right: width / 2 - 20,
      backgroundColor: '#ff9900',
      padding: 10,
      borderRadius: 6
    },
    plusIcon: {
      width: 21, // Adjust the width to resize the image
      height: 21, // Adjust the height to resize the image
    },
  });

export default Home;